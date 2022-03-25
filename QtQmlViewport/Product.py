from PyQt5.QtCore import pyqtSignal as Signal, pyqtProperty as Property, pyqtSlot as Slot, QObject, QTimer, QVariant, Q_CLASSINFO
from PyQt5.QtQml import QJSValue, QQmlListProperty
import traceback

class Setter(object):
    def __init__(self, classvars, name, init_value, callback = None, before_write_callback = None):
        self.property_name = name
        self.init_value = init_value
        self.attr_name = f'_{name}'
        self.signal_name = f'{name}Changed'

        

        self.callback_name = None
        self.callback = None
        self.before_write_callback_name = None
        self.before_write_callback = None
        
        if callback is not None:
            if callable(callback):
                if callback.__name__ in classvars: #is member function
                    self.callback_name = callback.__name__
                else:
                    self.callback = callback
            elif isinstance(callback, str) :
                if callable(getattr(classvars, callback)):
                    self.callback_name = callback
                else:
                    raise RuntimeError(f'{callback} is not a member function')
        if before_write_callback is not None:
            if callable(before_write_callback):
                if before_write_callback.__name__ in classvars: #is member function
                    self.before_write_callback_name = before_write_callback.__name__
                else:
                    self.before_write_callback = before_write_callback
            elif isinstance(before_write_callback, str) :
                if callable(getattr(classvars, before_write_callback)):
                    self.before_write_callback_name = before_write_callback
                else:
                    raise RuntimeError(f'{before_write_callback} is not a member function')

    def apply_callback(self, target):
        try:
            if self.callback_name is not None: #member function
                getattr(target, self.callback_name)() #preserves polymorphism
            elif self.callback is not None:
                self.callback() #standalone callback function
        except:
            print(traceback.format_exc())

    def _set(self, target, new_val):  #virtual
        new_val = cvt_if_js_value(new_val)
        if getattr(target, self.attr_name) != new_val:
            if self.before_write_callback is not None:
                value = self.before_write_callback(value)
            setattr(target, self.attr_name, new_val)
            getattr(target, self.signal_name).emit()
            self.apply_callback(target)

    def __call__(self, target, new_val):
        if not hasattr(target, self.attr_name):
            setattr(target, self.attr_name, self.init_value)
        self._set(target, new_val)


class InputSetter(Setter):
    '''
        To be used with InputProperty.
        If used, 'callback' may be
        - a member function
        - the name of a member function
        - a non-member function
        It will be called if (and only if) a new input is set.
    '''
    def __init__(self, classvars, name, init_value, callback = None, before_write_callback = None):
        super().__init__(classvars, name, init_value, callback, before_write_callback)
        
    def _set(self, target, new_val):
        if assign_input(target, self.property_name, new_val, self.before_write_callback):
            self.apply_callback(target)



class Getter(object):
    def __init__(self, name, init_value, set_producer):
        self.attr_name = f'_{name}'
        self.init_value = init_value
        self.set_producer = set_producer

    
    def _get(self, target): #virtual
        return getattr(target, self.attr_name)

    def __call__(self, target):
        if not hasattr(target, self.attr_name):
            setattr(target, self.attr_name, self.init_value)
            if self.set_producer and issubclass(type(target), Product):
                self.init_value.set_producer(target)

        return self._get(target)

class QVariantGetter(Getter):
    def __init__(self, name, init_value, set_producer):
        super().__init__(name, init_value, set_producer)

    def __call__(self, target):
        return QVariant(super().__call__(target))

class ConstGetter(object):
    def __init__(self, value):
        self.value = value

    def __call__(self, _):
        return self.value

def select_getter(typename, name, init_value, set_producer = False):
    if typename == QVariant:
        return QVariantGetter(name, init_value, set_producer)
    return Getter(name, init_value, set_producer)



def RWProperty(classvars, typename, name, init_value, callback = None, before_write_callback = None):
    '''
        This function adds a QProperty named 'name' to a class's vars() dictionary.
        It create the getter, setter, and signal named 'nameChanged'.

        *Important* a member variable named '_name' will be expected by the getter and setter.

        A QProperty is exposed to QML.
    '''
    notify = classvars[f'{name}Changed'] = Signal()
    classvars[f'{name}'] = Property(typename, select_getter(typename, name, init_value), Setter(classvars, name, init_value, callback, before_write_callback), notify = notify)

def ROProperty(classvars, typename, name, init_value):
    '''
        This function adds a QProperty named 'name' to a class's vars() dictionary.
        It creates the getter, and signal named 'nameChanged'. It also creates
        a set_name() setter outside of the Qt property system.

        *Important* a member variable named '_name' will be expected by the getter.

        A QProperty is exposed to QML.
    '''
    notify = classvars[f'{name}Changed'] = Signal()
    classvars[f'{name}'] = Property(typename, select_getter(typename, name, init_value), notify = notify)
    classvars[f'set_{name}'] = Setter(classvars, name, init_value)

def ConstProperty(classvars, typename, name, init_value):
    '''
        This function adds a QProperty named 'name' to a class's vars() dictionary.
        It create the getter.

        *Important* a member variable named '_name' will be expected by the getter.

        A QProperty is exposed to QML.
    '''
    
    classvars[f'{name}'] = Property(typename, select_getter(typename, name, init_value, issubclass(type(init_value), Product)), constant = True)



def InputProperty(classvars, typename, name, init_value, callback = None, before_write_callback = None):
    '''
        This function adds a QProperty named 'name' to a class's vars() dictionary.
        It create the getter, setter, and signal named 'nameChanged'.

        *Important* a member variable named '_name' will be expected by the getter and setter.

        'callback()->None' will be called if
        (and only if) a new value is set. see InputSetter for more information on 'callback'
        A QProperty is exposed to QML.
        An InputProperty is a property that turns a product dirty when needed.
        It can be a primitive type (e.g. int, string, bool, etc) or a Product,
        or a collection containing products

        'before_write_callback(value) -> value' will be called before assignment (an assignment operation will occurs only if new value is different from old value)
    '''
    notify = classvars[f'{name}Changed'] = Signal()
    classvars[f'{name}'] = Property(typename, select_getter(typename, name, init_value), InputSetter(classvars, name, init_value, callback, before_write_callback), notify = notify)


def Q_ENUMS_mock(classvars, enumclass): #do not use, PySide2 workaround

    values = [a for a in dir(enumclass) if not a.startswith('__') and not callable(getattr(enumclass,a))]

    for v in values:
        classvars[f'{v}'] = Property(int, ConstGetter(getattr(enumclass,v)), constant = True)



class Product(QObject):
    '''
    classdocs
    '''
    productDirty = Signal()
    productClean = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        '''
        Constructor
        '''
        self._dirty = False
        self._autoUpdate = False
        self._dependencies = []
        self._error = None
        self._producer = None
        self._children = []

    Q_CLASSINFO('DefaultProperty', 'children')


    @Property(QQmlListProperty)
    def children(self):
        return QQmlListProperty(QObject, self, self._children)

    @Property(QObject, notify = productClean)
    def bind(self):
        return self
    
    InputProperty(vars(), bool, 'autoUpdate', False)
    InputProperty(vars(), list, 'dependsOn', [])

    def set_dirty(self, d):
        if self._dirty != d:
            self._dirty = d
            self.dirtyChanged.emit()
        if self._dirty and self.autoUpdate:
            QTimer.singleShot(0, self.update) # schedule an update as soon as we go back to event loop, but not before

    dirtyChanged = Signal()
    dirty = Property(bool, Getter('dirty', False, False), set_dirty, dirtyChanged)


    def _update(self):
        '''
        update function to override
        '''
        pass

    @Slot()
    def update(self):

        if self.dirty:

            self._error = None

            for d in self._dependencies:
                if not d.update():
                    self._error = d._error
                    return False

            try:
                self._update()
            except Exception as e:
                self._error = e
                print(traceback.format_exc())

            self.makeClean()

        return self._error is None

    @Slot()
    def makeDirty(self):
        if not self.dirty:
            self.dirty = True
            self.productDirty.emit()

    @Slot()
    def makeClean(self):
        if self.dirty:
            self.dirty = False
            self.productClean.emit()


    def add_dependency(self, d):
        if d is not None:
            self._dependencies.append(d)
            d.productDirty.connect(self.makeDirty)
            self.makeDirty()

    def remove_dependency(self, d):
        if d is not None:
            if d in self._dependencies:
                self._dependencies.remove(d)
                d.productDirty.disconnect(self.makeDirty)
            self.makeDirty()

    def set_producer(self, producer):
        if self._producer is not None:
            raise RuntimeError("Error: tried to call set a set_producer() twice on " + str(self) + ".")
        if not issubclass(type(producer), Product):
            raise RuntimeError("Error: tried to call set a set_producer() with type " + type(producer) + ".")
        self._producer = producer
        self.add_dependency(producer)
        producer.productClean.connect(self.makeClean)


class VariantProduct(Product):
    def __init__(self, parent=None, variant=None):
        super().__init__(parent)
        self.variant = variant #calls the setter

    InputProperty(vars(), 'QVariant', 'variant', None)

    @Slot(str, result = QVariant)
    def dictField(self, name):
        self.update()
        if isinstance(self._variant, dict):
            return QVariant(self._variant[name])
        
        return None


################################################################################
############# Helpers ##########################################################



def default_cmp(lhs, rhs):
    try:
        return lhs != rhs
    except:
        return True

def array_cmp(lhs, rhs):
    if lhs is not None and rhs is not None:
        return lhs.__array_interface__ != rhs.__array_interface__
    else:
        return lhs is None and rhs is None

def cvt_if_js_value(value):
    if isinstance(value, QJSValue):
        return value.toVariant()
    return value

def recurse_types(f, value):
    if isinstance(value, (list, tuple)):
        for v in value:
            recurse_types(f, v)
    elif isinstance(value, dict):
        for _, v in value.items():
            recurse_types(f, v)
    elif issubclass(type(value), Product):
            f(value)



def assign_input(product, property_name, value, before_write_callback = None):

    value = cvt_if_js_value(value)

    compare = default_cmp

    if value is not None and hasattr(value, '__array_interface__'):
        compare = array_cmp

    variable_name = f"_{property_name}"
    assert(issubclass(type(product), Product))
    try:
        current = getattr(product, variable_name)
    except:
        pass
    try:
        rv = compare(current, value)
    except:
        print(traceback.format_exc())
        rv = True

    if  rv:
        if before_write_callback is not None:
            value = before_write_callback(value)
        recurse_types(product.remove_dependency, current)

        setattr(product, variable_name, value)

        recurse_types(product.add_dependency, value)

        product.makeDirty()

        signal = getattr(product, f"{property_name}Changed")
        signal.emit()
        return True
    return False


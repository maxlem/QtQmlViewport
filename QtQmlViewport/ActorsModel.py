
from QtQmlViewport.Actors import Actor, Actors
from QtQmlViewport.Geometry import Geometry
from QtQmlViewport.Transforms import Transform
from QtQmlViewport.Effect import Effect
from QtQmlViewport.Array import ArrayBase
from QtQmlViewport.Product import Product
import re
import sys
from PyQt5.QtGui import QVector3D
from PyQt5.QtCore import (
    QAbstractItemModel,
    QByteArray,
    QModelIndex,
    Qt,
    QObject,
    QVariant,
    pyqtProperty as Property,
    pyqtSignal as Signal,
    pyqtSlot as Slot
)
from enum import IntEnum, auto

class Roles(IntEnum):
    ActorsNameRole = Qt.UserRole
    NameRole = auto()

class QProperty(object):
    def __init__(self, qobject, metaproperty, row, model):
        self._qobject = qobject
        self._metaProperty = metaproperty
        self._row = row
        self._model = model
        self._index = QModelIndex()

        if self._metaProperty.hasNotifySignal():
            metaMethod = self._metaProperty.notifySignal()
            signal = getattr(self._qobject, metaMethod.name().data().decode('utf8'))
            signal.connect(self.notify)

    def setIndex(self, index):
        self._index = index

    def check(self):
        if self._qobject is None:
            raise Exception("actor is None")
        if self._metaProperty is None:
            raise Exception("metaProperty is None")
    def read(self):
        self.check()
        if not self._metaProperty.isReadable():
            raise Exception("not readable")

        return getattr(self._qobject, self._metaProperty.name())
    def write(self, value):
        self.check()
        if not self._metaProperty.isWritable():
            raise Exception("not writable")
        
        # if not isinstance(value, QVariant):
        #     raise Exception(f"wrong type: {type(value)}")

        setattr(self._qobject, self._metaProperty.name(), value)

    def name(self):
        self.check()
        return self._metaProperty.name()
    
    def objectName(self):
        return self._qobject.objectName()

    def type(self):
        return self._metaProperty.type()

    def owner(self):
        return self._qobject
    
    def leaf(self, leafTypes):
        if self.type() == QVariant.UserType:
            value = self.read()
            if isinstance(value, leafTypes):
                return value
        return None
    def row(self):
        return self._row

    def notify(self):

        self._model.beginResetModel()
        # self._model.dataChanged.emit(self._index, self._index)
        self._model.endResetModel()



ROLES_MAPPPING = {
    Roles.ActorsNameRole: "ActorsNameRole",
    Roles.NameRole: "NameRole",
}

# Example
#
# parent     |  type     |leaf| ActorsNameRole                           |     NameRole                                             
# ------     | --------- |---| ---------------------------------------- | ------------------------------ 
#  None      |  Actors   |   |- root                                    |   root name                                              
#  root      |  Actor    |   |   |- actorA                              |   actorA name                         
#  actorA    |  QProperty |   |   |    |- propertyA (int)                |   1234             
#  actorA    |  QProperty |   |   |    |- propertyB (float)              |   1234.0f            
#  actorA    |  QProperty |   |   |    |- propertyC (QVector3D)          |   QVector3D(1,2,3)             
#  actorA    |  QProperty |[x]|   |    |- propertyD (Geometry*)          |   geometry name            
#  propertyD |  QProperty |   |   |    |     |- propertyE (int)          |   1234             
#  propertyD |  QProperty |   |   |    |     |- propertyF (float)        |   1234.0f            
#  propertyD |  QProperty |   |   |    |     |- propertyG (QVector3D)    |   QVector3D(1,2,3)               
#  propertyD |  QProperty |[x]|   |    |     |- propertyH (Transform*)   |   transform name                                    |              
#  propertyH |  QProperty |   |   |    |           |- [...]              |   [...]           
#  actorA    |  QProperty |   |   |    |-[...]                           |   [...]
#  root      |  Actors   |   |   |- actorsA                             |   actorsA name           
#  actorsA   |  Actor    |   |   |     |- actorB                        |   actorB name
#  actorB    |  QProperty |   |   |           |-[...]                    |   [...]
#  root      |  Actor    |   |   |- actorC                              |   actorC name
#  actorC    |  QProperty |   |          |-[...]                         |   [...]

class ActorsModel(QAbstractItemModel):

    LeafTypes = (Geometry, Effect, Transform, ArrayBase)
    
    def __init__(self, parent = None):
        super().__init__(parent)
        self._offset = Product.staticMetaObject.propertyOffset()
        self._root = None
        self._rolenames = dict()
        self._rolenames[Qt.DisplayRole] = QByteArray(b"display")
        for role, name in ROLES_MAPPPING.items():
            self._rolenames[role] = QByteArray(name.encode())
        self._cache = {}

    rootChanged = Signal()
    @Property(Actors, notify=rootChanged)
    def root(self):
        return self._root

    @root.setter
    def root(self, root):
        self._root = root
        self.rootChanged.emit()

    def columnCount(self, index=QModelIndex()):
        return 1

    def roleNames(self):
        return self._rolenames

    def rowCount(self, parent):
        if parent.isValid():
            item = parent.internalPointer()
            if isinstance(item, Actors):
                return item.count
            elif isinstance(item, Actor):
                return item.metaObject().propertyCount() - self._offset
            elif isinstance(item, QProperty):
                value = item.leaf(self.LeafTypes)
                if value is not None:
                    return value.metaObject().propertyCount() - self._offset
            return 0
        else:
            return self.root.count

    def index(self, row, column, parent=QModelIndex()):

        if column != 0:
            return QModelIndex()
        if not self.hasIndex(row, column, parent):
            return QModelIndex()
        if parent.isValid():
            parentItem = parent.internalPointer()
            if isinstance(parentItem, Actors):
                print(f"{sys._getframe().f_lineno} index {(row,column)}  data {parentItem.actors[row]} ({parentItem.actors[row].objectName()})")
                return self.createIndex(row, column, parentItem.actors[row])
            elif isinstance(parentItem, Actor):
                qprop = self.cache(parentItem, row)
                index = self.createIndex(row, column, qprop)
                qprop.setIndex(index)
                print(f"{sys._getframe().f_lineno} index {(row,column)}  data {qprop} ({qprop.objectName()})")
                return index
            elif isinstance(parentItem, QProperty):
                value = parentItem.leaf(self.LeafTypes)
                if value is not None:
                    qprop = self.cache(value, row)
                    index = self.createIndex(row, column, qprop)
                    qprop.setIndex(index)
                    print(f"{sys._getframe().f_lineno} index {(row,column)}  data {qprop} ({qprop.objectName()})")
                    return index
                    
            return QModelIndex()
        else:
            print(f"{sys._getframe().f_lineno} index {(row,column)}  data { self.root.actors[row]} ({self.root.actors[row].objectName()})")
            return self.createIndex(row, column, self.root.actors[row])
        
    def cache(self, qobject, row):
        if not qobject in self._cache:
            self._cache[qobject] = {}
        if not row in self._cache[qobject]:
            self._cache[qobject][row] = QProperty(qobject, qobject.metaObject().property(row+self._offset), row, self)
        return self._cache[qobject][row]

    def parent(self, index):
        if not index.isValid():
            return QModelIndex()
        item = index.internalPointer()
        if item == self.root:
            return QModelIndex()
        if isinstance(item, (Actors, Actor)):
            parentItem = item.parent
            if not isinstance(parentItem, Actors):
                raise Exception(f"unexpected parent type {type(parentItem)}")
            return self.createIndex(parentItem.actors.index(item), 0, parentItem)
        elif isinstance(item, QProperty):
            return self.createIndex(item.row(), 0, item.owner())
        return QModelIndex()

    def data(self, index, role=Qt.DisplayRole):
        if not index.isValid():
            return
        item = index.internalPointer()
        if role == Qt.DisplayRole:
            return str(index.internalPointer())
        elif role == Roles.ActorsNameRole:
            if isinstance(item, (Actor, Actors)):
                return item.name
            elif isinstance(item, QProperty):
                return item.name()
            return
        elif role == Roles.NameRole:
            if isinstance(item, (Actor, Actors)):
                return str(item)
            if isinstance(item, QProperty):
                return item.read()
    
    def flags(self, index):
        flag = Qt.ItemIsEnabled | Qt.ItemIsSelectable
        item = index.internalPointer()
        if isinstance(item, QProperty) and item.type() in [QVariant.Bool, QVariant.String, QVariant.Double, QVariant.Vector3D]:
            flag |= Qt.ItemIsEditable
        return flag



    def setData(self, index, value, role = Qt.EditRole):

        if role != Qt.EditRole:
            return False

        item = index.internalPointer()
        if not isinstance(item, QProperty):
            raise Exception("bad index")
        
        rv = True

        if item.type() == QVariant.Bool:
            item.write(True if value == "true" else False)
        elif item.type() == QVariant.String:
            item.write(value)
        elif item.type() == QVariant.Int:
            item.write(int(value))
        elif item.type() == QVariant.Double:
            item.write(float(value))
        elif item.type() == QVariant.Vector3D:
            match = re.match(r"QVector3D\(([+-]?([0-9]*[.])?[0-9]+)\s*,\s*([+-]?([0-9]*[.])?[0-9]+)\s*,\s*([+-]?([0-9]*[.])?[0-9]+)\)", value)
            if match is None:
                print(f"bad value for type {item.type()}: {value}")
                rv = False
            else:
                item.write(QVector3D(float(match.group(1)), float(match.group(3)), float(match.group(5))))
        else:
            rv = False
            print(f"Unsupported type {item.type()}")
        
        # if rv: # something is wrong with dataChanged()
        #     self.beginResetModel()
            # self.dataChanged.emit(index, index)
            # self.endResetModel()
        
        return rv
            
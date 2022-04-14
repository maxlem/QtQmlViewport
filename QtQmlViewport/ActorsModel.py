
from QtQmlViewport.Actors import Actor, Actors
from QtQmlViewport.Geometry import Geometry
from QtQmlViewport.Transforms import Transform
from QtQmlViewport.Effect import Effect
from QtQmlViewport.Array import ArrayBase
from QtQmlViewport.Product import Product

from dataclasses import dataclass, field
from PyQt5.QtCore import (
    QAbstractItemModel,
    QByteArray,
    QModelIndex,
    Qt,
    QObject,
    QVariant,
    pyqtProperty as Property,
    pyqtSignal as Signal
)
from enum import IntEnum, auto

class Roles(IntEnum):
    ActorsNameRole = Qt.UserRole
    NameRole = auto()

class ItemProperty(object):
    def __init__(self, actor=None, metaproperty = None):
        self.actor = actor
        self.metaProperty = metaproperty
    def check(self):
        if self.actor is None:
            raise Exception("actor is None")
        if self.metaProperty is None:
            raise Exception("metaProperty is None")
    def read(self):
        self.check()
        if not self.metaProperty.isReadable():
            raise Exception("not readable")

        return getattr(self.actor, self.metaProperty.name())

    def name(self):
        self.check()
        return self.metaProperty.name()
    
    def type(self):
        return self.metaProperty.type()


ROLES_MAPPPING = {
    Roles.ActorsNameRole: "ActorsNameRole",
    Roles.NameRole: "name",
}

class ActorsModel(QAbstractItemModel):

    LeafTypes = (Actor, Geometry, Effect, Transform, ArrayBase)
    
    def __init__(self, parent = None):
        super().__init__(parent)
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
            elif isinstance(item, self.LeafTypes):
                return item.metaObject().propertyCount() - Product.staticMetaObject.propertyOffset()
            elif isinstance(item, ItemProperty):
                if item.type() == QVariant.UserType:
                    value = item.read()
                    if value is not None and isinstance(value, self.LeafTypes):
                        return 1
            return 0
        else:
            return self.root.count

    def index(self, row, column, parent=QModelIndex()):
        if column != 0:
            return QModelIndex()
        if not self.hasIndex(row, column, parent):
            return QModelIndex()
        if parent.isValid():
            item = parent.internalPointer()
            if isinstance(item, Actors):
                return self.createIndex(row, column, item.actors[row])
            elif isinstance(item, self.LeafTypes):
                offset = Product.staticMetaObject.propertyOffset()
                
                if not item in self._cache:
                    self._cache[item] = {}
                if not row in self._cache[item]:
                    self._cache[item][row] = ItemProperty(item, item.metaObject().property(row+offset))
                return self.createIndex(row, column, self._cache[item][row])
            elif isinstance(item, ItemProperty):
                if item.type() == QVariant.UserType:
                    value = item.read()
                    if value is not None and isinstance(value, self.LeafTypes):
                        return self.createIndex(row, column, value)
            return QModelIndex()
        else:
            return self.createIndex(row, column, self.root.actors[row])

    def parent(self, index):
        if not index.isValid():
            return QModelIndex()
        item = index.internalPointer()
        if isinstance(item, Actors):
            return QModelIndex()
        elif isinstance(item, self.LeafTypes):
            return self.createIndex(0, 0, item.parent)
        elif isinstance(item, ItemProperty):
            return self.createIndex(0, 0, item.actor)
        return QModelIndex()

    def data(self, index, role=Qt.DisplayRole):
        if not index.isValid():
            return
        item = index.internalPointer()
        if role == Qt.DisplayRole:
            return str(index.internalPointer())
        elif role == Roles.ActorsNameRole:
            if isinstance(item, Actors):
                return item.name
            elif isinstance(item, self.LeafTypes):
                return index.parent().internalPointer().name
            elif isinstance(item, ItemProperty):
                return item.name()
            return
        elif role == Roles.NameRole:
            if isinstance(item, self.LeafTypes):
                return item.objectName()
            elif isinstance(item, ItemProperty):
                return item.read()
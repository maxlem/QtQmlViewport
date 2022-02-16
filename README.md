# QtQmlViewport

(forked from pioneer.common.ui) This package contains QtQmlViewport which can be used to create a QtQml 3D and 2D user interface using python and QML language.

It also contains PyBVH which is a C++ multi-threaded BVH creation and query library that can be used for sophisticated picking queries on Triangles and Points BVH's


## Installation

Use the package manager to install QtQmlViewport

```bash
pip install QtQmlViewport
```
** Prerequisites **
To setup QtQmlViewport in develop mode, you need to have installed **cmake** beforehand.

**Eigen3** and **tbb** are also required (for PyBVH)
```bash
sudo apt install libeigen3-dev
sudo apt install libtbb-dev
```

When developing, you can link the repository to your python site-packages and enable hot-reloading of the package
```bash
pip3 install -r requirements.txt 
python3 setup.py develop --user
```

If you don't want to install all the dependencies on your computer, you can run it in a virtual environment as well.
```bash
pipenv install --skip-lock

pipenv shell
```

## Usage


```python

from QtQmlViewport import interactive, Actors, Geometry, Array, CustomEffects, CustomActors
from PyQt5.QtGui import QColor
import trimesh

trimesh_ = trimesh.load("tmp.stl")

scene_actor = Actors.Actor(
    geometry = Geometry.Geometry(
        indices = Array.Array(ndarray = trimesh_.faces.flatten().astype('u4'))
        , attribs = Geometry.Attribs(
            vertices = Array.Array(ndarray = trimesh_.vertices)
            , normals = Array.Array(ndarray = trimesh_.vertex_normals))
        , primitive_type = Geometry.PrimitiveType.TRIANGLES
    ), effect = CustomEffects.material(color = QColor("brown"), back_color = QColor("blue"), light_power = 5e1, light_follows_camera = True, shininess = 100.0, reverse_backfaces = False, flat_shading = True)
    , transform = CustomActors.ensure_Transform(None)
    , name = "actor"
)

vp = interactive.viewport("main.qml")
root = vp.root_wrapper()
root.actors.addActor(scene_actor)
vp.wait_key(" ")

```

# ComputerGraphicsProject2
This is a  project for a Computer Graphics course in NCTU


### Build Environment : 

- OS: Ubuntu 20.04.1 LTS

- Install libraries:

```bash
sudo apt-get install build-essential libgl1-mesa-dev g++
# install build tool and drivers

sudo apt-get install libglew-dev libglfw3-dev libglm-dev
# install GLFW and GLEW

```

- Check OpenGL version (I am working with version 4.6)
```bash
glxinfo | grep OpenGL
# You should find followings lines:
# OpenGL version string: 4.6 (Compatibility Profile) Mesa 20.0.8
# OpenGL shading language version string: 4.60
```
### Build The Project
```bash
make all
```
![alt text](Resources/bunny_moon.png?raw=true "玉兔美照")





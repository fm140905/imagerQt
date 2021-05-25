# Simple GUI application for back projection image reconstruction

## Prerequisites
- Install [ROOT v6](https://root.cern/install/).
- Install qt5 development packages
```bash
sudo apt-get install qt5-default qtdeclarative5-dev
```

## Build
  ```bash
  mkdir -p build
  cd build
  qmake -config release ../imagerQt.pro
  make
  ```
## Run
```bash
    cd ..
    build/ImagerQt
```

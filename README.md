# gui-qt 
![](https://api.travis-ci.com/holmos-frs/gui-qt.svg?branch=master)
Eine Qt-basierte GUI-Anwendung zur Auswertung von holographisch-mikroskopischer Aufnahmen.

## Installation
Das Programm hat mehrere Abhängigkeiten:

    * [OpenCV](https://opencv.org) mit C++11-Unterstützung für lambda-Ausdrücke
    * [fftw](http://fftw.org)
    * Qt 5

Um Qt und fftw auf einem Raspberry mit raspbian zu installieren, einfach folgende Kommandos ausführen:
```
sudo apt update && \
sudo apt install qt5-default qt5-qmake libfftw3-dev
```

Es gibt auch ein OpenCV-Build in den offiziellen Repos von Raspbian, allerdings ist diese Version vergleichsweise alt und unterstützt kein C++11, weshalb für gui-qt eine aktuellere OpenCV-Version installiert werden muss.

Um OpenCV automatisiert zu kompilieren und installieren, einfach das `deps/install-opencv.sh`-Skript ausführen.

## Compilation
Nach Installation oben genannter Dependencies sind folgende Kommandos im Projekt-Verzeichnis auszuführen:
```
qmake && \
make -j4 && \
sudo make install
```

## Docker-Build
Um die gui-qt-Builds reproduzierbar und cross-kompilierbar zu machen, wird ein Docker-Image zur Verfügung gestellt, das auf jedem beliebigen Docker-unterstützten x86-System ausgeführt werden kann.
Mehr Informationen zur Installation und Benutzung von Docker gibt es auf der offiziellen [Projektseite](https://www.docker.com).

Um die cross-compilation und dependencies vorzubereiten, muss das Docker-Image zuerst gebaut werden:
```
docker run --rm --privileged multiarch/qemu-user-static:register --reset
docker build -t holmosbuild .
```
Dies kann eine ganze Weile dauern, die darauffolgenden Builds von gui-qt sind aber drastisch schneller.

Im Projektverzeichnis kann dann folgender Command ausgeführt werden, um gui-qt für den Raspberry zu bauen:
```
docker run -v $(pwd):/build holmosbuild
```

Falls alles glatt läuft, befindet sich nun eine `holmos-viewer.deb`-Datei im Verzeichnis, dass auf dem Raspberry Pi folgendermaßen installiert werden kann:
```
pi@raspberrypi$ sudo dpkg -i holmos-viewer.deb
pi@raspberrypi$ sudo apt-get -f install
```

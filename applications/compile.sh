#!/bin/sh

cd calculator/
bash build.sh $1
cd ..

cd CandyGUI/
bash build.sh $1
cd ..

cd CandyNote/
bash build.sh $1
cd ..

cd CandyShell/
bash build.sh $1
cd ..

cd CandyTerminal
bash build.sh $1
cd ..

cd cpudet/
bash build.sh $1
cd ..

cd exec/
bash build.sh $1
cd ..

cd idle/
bash build.sh $1
cd ..

cd init/
bash build.sh $1
cd ..

cd ist/
bash build.sh $1
cd ..

cd mx/
bash build.sh $1
cd ..

cd MXLogin/
bash build.sh $1
cd ..

cd pcitool/
bash build.sh $1
cd ..

cd ps/
bash build.sh $1
cd ..

cd ps2driver/
bash build.sh $1
cd ..

cd spawner/
bash build.sh $1
cd ..

cd timedriver/
bash build.sh $1
cd ..

cd UItweak/
bash build.sh $1
cd ..

cd uname/
bash build.sh $1
cd ..

cd utils/
bash build.sh $1
cd ..

cd vesadriver/
bash build.sh $1
cd ..

cd welcome/
bash build.sh $1
cd ..

cd zipNET/
bash build.sh $1
cd ..

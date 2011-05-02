echo "**** Build of configuration Release for project wavEncrypt ****"

rm -r Release
mkdir Release
mkdir Release/src

# make all 
echo "Building file: ./src/crypto.c"
gcc -I"./include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Release/src/crypto.d" -MT"./Release/src/crypto.d" -o"./Release/src/crypto.o" "./src/crypto.c"
echo "Finished building: ./src/crypto.c"
 
echo "Building file: ./src/cryptoTest.c"
gcc -I"./include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Release/src/cryptoTest.d" -MT"./Release/src/cryptoTest.d" -o"./Release/src/cryptoTest.o" "./src/cryptoTest.c"
echo "Finished building: ./src/cryptoTest.c"
 
echo "Building file: ./src/dataHolder.c"
gcc -I"./include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Release/src/dataHolder.d" -MT"./Release/src/dataHolder.d" -o"./Release/src/dataHolder.o" "./src/dataHolder.c"
echo "Finished building: ./src/dataHolder.c"
 
echo "Building file: ./src/main.c"
gcc -I"./include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Release/src/main.d" -MT"./Release/src/main.d" -o"./Release/src/main.o" "./src/main.c"
echo "Finished building: ./src/main.c"
 
echo "Building file: ./src/parserADT.c"
gcc -I"./include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Release/src/parserADT.d" -MT"./Release/src/parserADT.d" -o"./Release/src/parserADT.o" "./src/parserADT.c"
echo "Finished building: ./src/parserADT.c"
 
gcc -I"./include" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"./Release/src/wav.d" -MT"./Release/src/wav.d" -o"./Release/src/wav.o" "./src/wav.c"
echo "Finished building: ../src/wav.c"
 
echo "Building target: wavEncrypt"
gcc  -o "./Release/wavEncrypt"  ./Release/src/crypto.o ./Release/src/cryptoTest.o ./Release/src/dataHolder.o ./Release/src/main.o ./Release/src/parserADT.o ./Release/src/wav.o   -lcrypto
echo "Finished building target: wavEncrypt :D"

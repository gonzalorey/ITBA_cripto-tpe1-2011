----------------------------------------------------------------------------------------------------
README.txt

Grupo 2: Mangiarotti, Nizzo Mc Intosh, Rey
----------------------------------------------------------------------------------------------------

Compilacion:

Para compilar el programa hay que posicionarse en el directorio raíz, donde se encuentra el archivo makefile.
Abrir una terminal y ejecutar

$>> ./makefile

Se crea un directorio Release/
En este directorio se encuentra el archivo ejecutable wavEncrypt
Este es el archivo utilizado para ejecutar el programa

----------------------------------------------------------------------------------------------------

Ejemplos de ejecucion:

Dichos comandos se ejecutan correctamente estando posicionado sobre el directorio Release/

i)		./wavEncrypt -in ../wav/incogDES.wav -out ../wav/incogDESOrig.wav -K 12345678 -iv 87654321 -m ecb -d -a des
ii) 	./wavEncrypt -in ../wav/fun2AESCFB.wav -out ../wav/fun2AESCFBOrig.wav -d -a aes128 -m cfb -pass "sorpresa"
iii)	./wavEncrypt -in ../wav/do-si.wav -out ../wav/do-siEncriptado.wav -e -a des -m ecb -pass "sorpresa"
iv)		./wavEncrypt -in ../wav/la-mi.wav -out ../wav/la-miEncriptado.wav -e -a aes128 -m cfb -pass "sorpresa"

----------------------------------------------------------------------------------------------------
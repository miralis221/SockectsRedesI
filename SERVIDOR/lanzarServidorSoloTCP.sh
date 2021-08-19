#!/bin/bash

./servidor
./cliente localhost TCP ordenes/ordenesprueba.txt &
./cliente localhost TCP ordenes/ordenesprueba2.txt &
./cliente localhost TCP ordenes/ordenesprueba3.txt


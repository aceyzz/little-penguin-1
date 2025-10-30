## Quelques commandes utiles pour la soutenance

### Assignment 00

Kernel Origin
```bash
cd /usr/src/linux-linus
git remote -v
```
puis check le fichier log push pour voir la version du kernel booté  

Configuration
```bash
cat /boot/config-6.18.0-rc2-00236-g566771afc7a8 | grep CONFIG_LOCALVERSION_AUTO
```
> doit retourner `CONFIG_LOCALVERSION_AUTO=y`

<br>

### Assignment 01

Hard stuff
```bash
dmesg -C
cd ~/a01
make
insmod main.ko
rmmod main.ko
dmesg -T | tail -20
```
> doit retourner les messages du module "Hello, world!" et "Cleaning up module."

<br>

### Assignment 02

Thor, god of thunder  
> Checker le fichier de log pour voir apparaitre la version de kernel avec le suffixe `-thor_kernel`  

We love Patches  
```bash
perl /usr/src/linux-linus/scripts/checkpatch.pl --strict --patch ~/a02/makefile-thor_kernel.patch
```
> doit retourner `Total errors: 0`
> fonctionnement démontré dans le fichier de log, car la EXTRAVERSION a bien été modifiée (suffixe du boot)

<br>

### Assignment 03

Love pedantic
```bash
cd ~/a03
perl /usr/src/linux-linus/scripts/checkpatch.pl -no-tree -file --strict main.c
```
> doit retourner `Total errors: 0`

<br>

### Assignment 04

A keyboard ? What for ?
> Check le fichier .rules (règle `udev`)
```bash

```
> Test compliqué car pas de port usb (autre que usb-c, et jai pas de clavier usb-c) sur mon mac, par contre log pour prouver que c'est fonctionnel, parce que jai une docking usb a la maison avec un clavier usb classique dessus

<br>

### Assignment 05

A real kernel developper
```bash
cd ~/a05
make test
make test-log
norme fortytwo.c
```
> check l'output pour verifier que tout les tests passent

readFTW
```bash
make
insmod fortytwo.ko
cat /dev/fortytwo && echo "" # doit retourner cedmulle
echo 123 > /dev/fortytwo # doit retourner une erreur
echo -n cedmulle > /dev/fortytwo # doit etre valide, checker avec 'echo $? ' qui doit retourner 0
rmmod fortytwo.ko
dmesg -T | tail -n 2
```
> `make test` et `make test-log` font deja toutes les verifs, mais ces commandes sont pour un test a la mano

<br>

### Assignment 06

Terminator
> Montrer le fichier de log pour prouver le boot sur le kernel linux-next, puis check le repo git du kernel
```bash
cd /usr/src/linux-next
git remote -v
```

Linux next est une branche de développement du kernel linux qui intègre les dernières modifications en cours avant leur inclusion dans la branche principale (celle vue dans l'assignment 00 de Linus Torvald)  

<br>

### Assignment 07

Creation and Deletion
```bash
cd ~/a07
make test
make test-log
norme dbfs.c
```
> check l'output pour verifier que tout les tests passent et conforme au sujet
> test de lock : j'utilise des mutex pour que l'ecriture ne soit possible en meme temps que la lecture, et vice versa

id
> doit etre readable et writable par tout les users

jiffies
> read-only pour tout les users

foo
> writable seulement par root, avec lock pour ecriture/lecture

<br>

### Assignment 08

Coding style, again ?!
```bash
cd ~/a08
norme reverse.c
```
> doit retourner `Total errors: 0`

Indiana Jones Theme
```bash
make test
```
> check l'output pour verifier que tout les tests passent

<br>

### Assignment 09

Almost real
```bash
cd ~/a09
make
norme mymounts.c
insmod mymounts.ko
cat /proc/mymounts
rmmod mymounts.ko
dmesg -T | tail -n 2
```
> doit retourner la liste des systèmes de fichiers montés, similaire à la commande `mount`


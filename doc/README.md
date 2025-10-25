Ce document a pour utilité de feuille de route sur ce que j'ai réalisé dans le cadre du projet, étape par étape. Ma VM (construite via le projet ft_linux) tourne sur UTM, et mon hôte est un MacBook Pro M3 sous MacOS Tahoe 26.1.

## Assignment 00

<details>
<summary>Voir le détail</summary>

#### Objectif

Télécharger, build, installer et booter sur la derniere version du kernel Linux faite par Linus Torvalds sur git.kernel.org

#### À rendre

- Le fichier de log du Kernel Boot  
- Le fichier de configuration du kernel utilisé  

#### Étapes

> dans la VM

1. Récuperer le Git tree de Linus

```bash
cd /usr/src
git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git linux-linus
cd linux-linus
git rev-parse --short HEAD
make -s kernelversion
```

2. Nettoyage et configuration du kernel avec l'ancienne config

```bash
make mrproper
cp -v /boot/config-6.16.1 .config
yes "" | make oldconfig
```

3. Ajotuer les configs specifiques a l'exercice

```bash
make menuconfig
# General setup  --->
#   Local version - append to kernel release  --->  laisser vide
#   Automatically append version information to the version string  --->  [*]
# Device Drivers --->
#   Graphics support --->
#     [*] Direct Rendering Manager (XFree86 4.1.0 and higher DRI support)
#     [*] Virtio GPU driver
```

4. Compiler le kernel et les modules

```bash
make -j$(nproc)
make modules_install
```

4.a Verification de la compilation

```bash
make -s kernelrelease
# Doit retourner : 6.18.0-rc2-...-g<sha>
ls -lh arch/arm64/boot
file arch/arm64/boot/Image
# L’Image doit être de type : ARM aarch64 kernel Image
```

5. Copier le noyau dans boot

```bash
KREL="$(make -s kernelrelease)"
mountpoint -q /boot || mount /boot
install -m0644 arch/arm64/boot/Image "/boot/Image-${KREL}"
install -m0644 System.map             "/boot/System.map-${KREL}"
install -m0644 .config                "/boot/config-${KREL}"
ls -lh /boot | grep "${KREL}"
```

6. Mettre a jour la config de GRUB

Environnement
```bash
UUID="$(blkid -s UUID -o value "$(findmnt -no SOURCE /)")"
KREL="$(make -s kernelrelease)"
PARTUUID="557f28c8-5006-6f4a-b73d-eb11e6468a1d"
```

Backup de la config existante
```bash
cp -av /boot/grub/grub.cfg /boot/grub/grub.cfg.bak.$(date +%F-%H%M%S)
```

Ajout de la nouvelle entrée dans grub.cfg
```bash
cat >> /boot/grub/grub.cfg <<EOF

menuentry "Little-Penguin-00 (${KREL})" {
    linux /vmlinuz-${KREL} root=PARTUUID=557f28c8-5006-6f4a-b73d-eb11e6468a1d ro console=tty1 console=ttyAMA0 earlyprintk=efi,keep ignore_loglevel
}
EOF
```

Verification
```bash
cat /boot/grub/grub.cfg
```

7. Rebooter sur le nouveau kernel

> reboot la VM

Selectionner le kernel "Little-Penguin-00" dans le menu GRUB
Verifier avec 
```bash
uname -r
# Doit retourner : 6.18.0-rc2-...-g<sha>
```

8. Exporter les fichiers demandés

```bash
dmesg -T > "/root/kernel-boot.log"
cp -v "/boot/config-$(KREL)" /root/.config
```

> Retourner sur le machine hote et recuperer les fichiers dans /root de la VM via scp

```bash
scp -P <port_ssh> root@<VM_IP_ADDRESS>:/root/kernel-boot.log .
scp -P <port_ssh> root@<VM_IP_ADDRESS>:/root/.config .
```

Puis déplacer les fichiers dans le dossier de rendu du projet

Et voila, le machtou pichtou

</details>

<br>

## Assignment 01

<details>
<summary>Voir le détail</summary>

#### Objectif

Créer un module kernel simple qui affiche un message dans le log du kernel lors de son insertion et de son retrait, en faisant attention a la compatibilité des versions

#### À rendre

- Le code source du module kernel  
- Le fichier Makefile pour compiler le module

#### Étapes

1. Coder le fichier .c du module et son Makefile

> [Code source ici](../project/01/)

2. Compiler le module

```bash
make
```

3. Insérer le module dans le kernel

```bash
sudo insmod main.ko
dmesg -T | tail -n 1
# doit afficher
# [timestamp] Hello world!
```
> le flag -T permet d'avoir des timestamps lisibles

4. Retirer le module du kernel

```bash
sudo rmmod main.ko
dmesg -T | tail -n 1
# doit afficher
# [timestamp] Cleaning up module.
```

5. Exporter les fichiers demandés

> Tu connais la procedure en `scp`

Et voila, le machtou pichtou

</details>

<br>

## Assignment 02

<details>
<summary>Voir le détail</summary>

#### Objectif

Reprendre le kernel compilé dans l'Assignment 00 et modifier le Makefile pour changer le champ EXTRAVERSION, pour ajouter le suffixe "-thor_kernel".

#### À rendre

- Le boot log du kernel
- Le patch au Makefile modifié

#### Étapes

1. Preparer la config

```bash
cd /usr/src/linux-linus
cp -v /boot/config-6.18.0-rc2-00236-g566771afc7a8 .config
yes "" | make oldconfig
```

2. Modifier le Makefile

Modifier EXTRAVERSION pour ajouter -thor_kernel
```bash
# si deja defini
if grep -qE '^EXTRAVERSION[[:space:]]*=' Makefile; then
  sed -i 's/^\(EXTRAVERSION[[:space:]]*=[[:space:]]*[^#\n]*\)/\1-thor_kernel/' Makefile
else
  # sinon add au debut du fichier
  sed -i '1i EXTRAVERSION = -thor_kernel' Makefile
fi
```

Check
```bash
grep -n '^EXTRAVERSION' Makefile
make -s kernelrelease
```
> Si il y a un suffixe "-thor_kernel" dans le retour, c'est bon. Ignorer le "-dirty" si present, ca veut juste dire que des fichiers ont été modifiés depuis la derniere compilation

3. Compiler le kernel et les modules

```bash
make -j"$(nproc)"
make modules_install
```

4. Copier le noyau dans boot

```bash
KREL="$(make -s kernelrelease)"
cp -v arch/arm64/boot/Image "/boot/Image-${KREL}"
cp -v System.map             "/boot/System.map-${KREL}"
cp -v .config                "/boot/config-${KREL}"
ls -lh /boot | grep "${KREL}"
```

5. Mettre a jour la config de GRUB

ajouter cette entrée a la fin de /boot/grub/grub.cfg

```
menuentry "Little-Penguin-02 (Image 6.18.0-rc2-thor_kernel)" {
    linux /Image-6.18.0-rc2-thor_kernel+ root=PARTUUID=557f28c8-5006-6f4a-b73d-eb11e6468a1d ro console=tty1 console=ttyAMA0 earlyprintk=efi,keep ignore_loglevel
}
```
> Adapter les valeurs selon votre archi, prenez exemple sur l'entrée Little-Penguin-00

6. Rebooter sur le nouveau kernel

> reboot la VM

Selectionner le kernel "Little-Penguin-02" dans le menu GRUB  

Verifier avec 
```bash
uname -r
# Doit retourner : 6.18.0-rc2-thor_kernel
```
> peut changer selon version exacte compilée

7. Créer le fichier patch

```bash
cd /usr/src/linux-linus
git add Makefile
git commit -s -m "Makefile: append -thor_kernel to EXTRAVERSION

Add -thor_kernel to EXTRAVERSION so the running kernel reports the
required suffix for assignment 02."
git format-patch -1 --base=auto --stdout > ../makefile-thor_kernel.patch
```
> oui mon message de commit est genere par chatGPT, j'suis pas inspiré
> le flag -s sert a ajouter la ligne "Signed-off-by: ..." automatiquement, et le base auto sert a eviter les conflits de format (bonnes pratiques selon [la doc ici](https://github.com/torvalds/linux/blob/master/Documentation/process/submitting-patches.rst))

Check
```bash
ls -lh ../makefile-thor_kernel.patch
head -n 25 ../makefile-thor_kernel.patch
git show --stat
```

8. Exporter les fichiers demandés

> Maintenant tu connais la procedure en `scp`

</details>

<br>

## Assignment 03

<details>
<summary>Voir le détail</summary>

#### Objectif

Modifier le fichier .c donné pour qu'il soit compliant avec le [Linux Kernel Coding Style (LKCS)](https://www.kernel.org/doc/html/v4.10/process/coding-style.html)

#### À rendre

- Le code source modifié du module kernel

#### Étapes

1. Récupérer le code source de base

2. Créer un fichier .c dans le dossier du kernel

```bash
cd /usr/src/linux-linus
# vim ou copy comme tu veux
# perso jai fait un fichier `moche.c`
```

3. Analyser les erreurs de style

```bash
./scripts/checkpatch.pl --no-tree --strict --show-types --file moche.c
```

4. Corriger les erreurs de style

Suivre chaque remarque de checkpatch.pl et modifier le code source en conséquence, un peu comme avec la norminette (ehhhh ouaaaais, casse la tete)  
Mon implémentation corrigée est [ici](../project/03/main.c)

5. Re-analyser le code corrigé

```bash
./scripts/checkpatch.pl --no-tree --strict --show-types --file moche.c
# doit retourner :
#   total: 0 errors, 0 warnings, 0 checks, 38 lines checked
# 
#   moche.c has no obvious style problems and is ready for submission.
```

6. Exporter les fichiers demandés

> Tu connais la procedure en `scp`

</details>

<br>

## Assignment 04

> En cours de réalisation
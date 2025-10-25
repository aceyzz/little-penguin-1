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

[Code source ici](../project/a01/)

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

## Assignment 02

> En cours de réalisation
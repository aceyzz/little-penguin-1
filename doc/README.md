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

0. Explications

Le but de cet exercice est de se familiariser avec la compilation du kernel Linux a partir des sources officielles. Nous allons cloner le dépôt git officiel de Linus Torvalds, configurer le kernel avec une config existante (celle de notre LFS), ajouter quelques options spécifiques, compiler le kernel et les modules, installer le tout, mettre a jour GRUB, et rebooter sur le nouveau kernel. En fait, c'est exactement ce que nous avons fait dans ft_linux, mais cette fois-ci nous allons le faire a partir des sources officielles de Linus Torvalds.

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

0. Explications

Un module kernel est un morceau de code qui peut être chargé et déchargé dynamiquement dans le noyau Linux. Cela permet d'ajouter des fonctionnalités au noyau sans avoir a recompiler tout le noyau. Dans cet exercice, nous allons créer un module kernel simple qui affiche un message dans le log du kernel lors de son insertion et de son retrait. Nous allons également nous assurer que le module est compatible avec la version du noyau en cours d'exécution.  

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

0. Explications

Le but de cet exercice est de modifier le Makefile du kernel Linux pour changer le champ EXTRAVERSION, en ajoutant le suffixe "-thor_kernel". Cela permettra d'identifier facilement le kernel compilé pour cet exercice. Nous allons ensuite recompiler le kernel avec cette modification, installer le nouveau kernel, mettre a jour GRUB, et rebooter sur le nouveau kernel. Enfin, nous allons créer un patch git pour la modification du Makefile, et exporter le fichier pour preuve dans le rendu.  

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

0. Explications

Le but de cet exercice est de modifier un fichier source C donné pour qu'il soit conforme au **Linux Kernel Coding Style (LKCS)**. Nous allons utiliser l'outil `checkpatch.pl` fourni avec les sources du kernel pour analyser le code et identifier les violations du style (un peu comme on faisait avec la norminette). Ensuite, on corrigera ces violations en suivant les recommandations du LKCS. Puis on exportera le tout pour le rendu.   

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

(optionnel)
J'ai fais un alias dans mon `~/.bashrc` pour faire une commande `norme` qui lance checkpatch.pl avec les bons flags, utile pour coder tout les modules kernel en respectant le LKCS
```bash
vim ~/.bashrc
# ajouter a la fin du fichier :
alias norme='/usr/src/linux-linus/scripts/checkpatch.pl --no-tree --strict --show-types --file'
```
> adapter le chemin selon votre arborescence
Puis recharger le bashrc
```bash
source ~/.bashrc
```
et vous pourrez faire
```bash
norme mon_module.c
```

6. Exporter les fichiers demandés

> Tu connais la procedure en `scp`

</details>

<br>

## Assignment 04

<details>
<summary>Voir le détail</summary>

#### Objectif

Modifier le module kernel fait dans [l'assignment 01](#assignment-01) pour qu'il soit chargé automatiquement lorsqu'un clavier USB est branché sur la machine

#### À rendre

- Le code source modifié du module kernel  
- Un "rules file" `udev` pour charger le module automatiquement  
- Une preuve (log) que le module a bien été chargé automatiquement  

#### Étapes

0. Explications

Le but de cet exercice est de modifier le module kernel créé dans l'assignment 01 pour qu'il soit chargé automatiquement lorsqu'un clavier USB est branché sur la machine. Pour cela, nous allons créer une règle `udev` qui détecte l'insertion d'un clavier USB et charge le module en conséquence. On modifiera legerement le code source pour le differencier du module 01. Enfin, nous testerons le tout en branchant un clavier USB et en vérifiant les logs du kernel pour confirmer que le module a été chargé automatiquement. Puis apres comme d'hab > exporter les fichiers pour le rendu  

1. Créer une règle `udev`

> [Voir règle dans le dossier](../project/04)

A placer dans `/etc/udev/rules.d`


2. Ajuster le code source du module

> [Code source ici](../project/04/)

3. Compiler le module

```bash
make
```
> J'ai repris un peu la meme logique qu'on avait faite dans ft_linux

4. Recharger les règles udev

```bash
udevadm control --reload-rules
```

Optionnel mais utile pour proof : clean l'historique
```bash
dmesg -c >/dev/null 2>&1 || true
```

5. Tester

```bash
# brancher un clavier USB, puis verifier les logs
dmesg -T | tail -n 5
```

6. Exporter les fichiers demandés

```bash
dmesg -T > plug.log
```
> Tu connais la procedure en `scp`

</details>

<br>

## Assignment 05

<details>
<summary>Voir le détail</summary>

#### Objectif

Reprendre le module kernel fait dans l'assignment 01 et le transformer en driver de caractère “misc” (interface simplifiée) qui expose un fichier de device `/dev/fortytwo` avec des opérations `read` et `write`

#### À rendre

- Le code source modifié du module kernel
- Une forme de proof (log) que le module fonctionne correctement

#### Étapes

0. Explications 

Un device permet a l'user space (applications, shell, etc) d'interagir avec le kernel via des fichiers spéciaux dans `/dev`. Creer un **character device** permet la communication caractere par caractere, comme un flux de données. Le kernel fournit une interface simplifiée pour créer des character devices appelés "misc devices" (miscellaneous devices), contrairement aux character devices classiques qui demandent plus de gestion (allocation de major/minor numbers, etc).  
Ce que nous devons faire, c'est de transformer notre module kernel en un **misc device** qui expose un fichier `/dev/fortytwo`.  
Quand on lit depuis ce fichier, on doit obtenir la valeur de notre login (pour ma part, `cedmulle`).  
- Si on ecrit dedans autre chose que notre login, le module doit retourner une erreur `invalid value`.  
- Si on ecrit notre login, le module doit accepter l'ecriture et stocker la valeur.  

1. Modifier le code source du module

> [Code source ici](../project/05)  

2. Compiler le module

```bash
make
```

3. Tester le module

J'ai fait une regle de Makefile pour automatiser les tests
```bash
make test
```

Possible aussi de faire une version plus verbose pour obtenir une preuve sous forme de fichier .log pour le rendu
```bash
make test-log > test.log 2>&1
```

4. Exporter le travail

> Tu connais la procedure en `scp`

</details>

<br>

## Assignment 06

<details>
<summary>Voir le détail</summary>

#### Objectif

Build (encore) et installer un kernel linux `linux-next` a partir des sources officielles. Un peu comme l'assignment 00, mais cette fois-ci avec le tree `linux-next` qui contient les features en cours d'intégration dans le kernel principal.

#### À rendre

- Le boot log du kernel

#### Étapes

0. Explications

La branche linux-next est une branche spéciale du dépôt git du kernel Linux qui contient les dernières modifications et fonctionnalités en cours d'intégration dans le kernel principal. Elle est mise à jour régulièrement avec les contributions des développeurs avant qu'elles ne soient fusionnées dans la branche principale. [Plus d'infos ici](https://www.kernel.org/doc/man-pages/linux-next.html)  
Le but de cet exercice est de compiler et d'installer un kernel à partir de cette branche linux-next, afin de se familiariser avec le processus de compilation du kernel et d'explorer les dernières fonctionnalités en développement.  

1. Récupérer le Git tree de linux-next

```bash
cd /usr/src
git clone https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git linux-next
cd linux-next
git remote add linux-next https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git
git fetch linux-next --tags
LATEST_NEXT_TAG="$(git tag -l 'next-*' | sort -V | tail -1)"
git switch -c lp06 "${LATEST_NEXT_TAG}"
git describe --tags --always
```
> Nous avons maintenant la derniere version de linux-next checkout

2. Configuration du kernel

> Je reprends mon ancienne config LFS, comme d'hab
```bash
make mrproper
cp -v /boot/config-6.16.1 .config
yes "" | make oldconfig
# desactiver le suffixe custom, et s'assurer que le suffixe auto est activé
scripts/config --set-str LOCALVERSION ""
scripts/config --enable LOCALVERSION_AUTO || true
```

Ajouter le driver Virtio GPU (sinon le terminal ne s'affiche pas au boot)
```bash
make menuconfig
# Device Drivers --->
#   Graphics support --->
#     [*] Direct Rendering Manager (XFree86 4.1.0 and higher DRI support)
#     [*] Virtio GPU driver
```
> Pas en module, faut que ce soit compilé dans le kernel


3. Build le kernel + modules

```bash
make -j"$(nproc)"
make modules_install
```

4. Copier le noyau dans boot

```bash
KREL="$(make -s kernelrelease)"
mountpoint -q /boot || mount /boot
install -m0644 arch/arm64/boot/Image "/boot/Image-${KREL}"
install -m0644 System.map             "/boot/System.map-${KREL}"
install -m0644 .config                "/boot/config-${KREL}"
ln -sf "Image-${KREL}" "/boot/vmlinuz-${KREL}"
ls -lh /boot | grep "${KREL}"
file "arch/arm64/boot/Image"
```

5. Mettre a jour la config de GRUB

Ajouter cette entrée a la fin de /boot/grub/grub.cfg

```
menuentry "Little-Penguin-06 (Image 6.18.0-rc2-next-20251024)" {
    linux /vmlinuz-6.18.0-rc2-next-20251024 root=PARTUUID=557f28c8-5006-6f4a-b73d-eb11e6468a1d ro console=tty1 console=ttyAMA0 earlyprintk=efi,keep ignore_loglevel
}
```
> Adapter les valeurs selon votre archi, prenez exemple sur l'entrée Little-Penguin-00

6. Rebooter sur le nouveau kernel

> reboot la VM
Selectionner le kernel "Little-Penguin-06" dans le menu GRUB
Verifier avec 
```bash
uname -r
# Doit retourner : 6.18.0-rc2-next-20251024
```
> peut changer selon version exacte compilée (date a laquelle vous faite cet assignment)

7. Exporter les fichiers demandés

```bash
dmesg -T > "/root/kernel-boot.log"
```
> Tu connais la procedure en `scp`

</details>

<br>

## Assignment 07

<details>
<summary>Voir le détail</summary>

#### Objectif

Coder un module kernel pour créer une interface debugfs appelée fortytwo qui expose trois fichiers virtuels : id, jiffies et foo, en utilisant le systeme de fichiers debugfs du kernel Linux.

#### À rendre

- Le code source du module kernel
- Une preuve (log) que le module fonctionne correctement

#### Étapes

0. Explications

Le but de cet exercice est de créer un module kernel qui utilise l'interface debugfs pour exposer des fichiers virtuels permettant d'interagir avec le noyau Linux. Debugfs est un système de fichiers spécial conçu pour le débogage et l'inspection du noyau, offrant une interface simple pour accéder aux données du noyau depuis l'espace utilisateur sans avoir a modifier le code du noyau lui-même ou utiliser des appels système complexes.  
Nous allons créer un module kernel qui crée un répertoire debugfs appelé `fortytwo`, contenant trois fichiers virtuels : `id`, `jiffies` et `foo`. Chaque fichier aura des fonctionnalités spécifiques pour lire et écrire des données.  

1. Verifier la présence de debugfs, sinon le monter

```bash
mount | grep debugfs || mount -t debugfs none /sys/kernel/debug
```

2. Coder le fichier .c du module et son Makefile

> [Code source ici](../project/07)

3. Compiler le module

```bash
make
```

4. Tester le module

```bash
make test
```
> Possible aussi de faire une version plus verbose pour obtenir une preuve sous forme de fichier .log pour le rendu
```bash
make test-log > test.log 2>&1
```

5. Exporter le travail

> Tu connais la procedure en `scp`

</details>

<br>

## Assignment 08

<details>
<summary>Voir le détail</summary>

#### Objectif

On nous donne un fichier .c d'un module kernel très mal implémenté. Il faut tout d'abord trouver la logique de ce que le gars a voulu implémenter, puis corriger + refactoriser le code pour qu'il soit fonctionnel, propre et respecte le Linux Kernel Coding Style (LKCS).

#### À rendre

- Le code source modifié du module kernel

#### Étapes

Plutot que des étapes, voici le changelog de ce que j'ai pu trouver comme erreurs et les corriger dans le code source :  

-	j’ai commencé par passer le fichier dans `checkpatch.pl`, ça m’a sorti une avalanche d’erreurs de style et de syntaxe, donc première étape : tout rendre compilable et lisible  
-	j’ai viré la licence bidon “LICENSE” et ajouté le header SPDX, c’est obligatoire pour tout module kernel récent  
-	j’ai trié et complété les includes : certains manquaient (`mutex`, `string`, `uaccess`) et d’autres étaient mal ordonnés  
-	j’ai supprimé les commentaires inutiles et le ton “fun” (wtf?)  

A partir de la, j'ai compris que le programme devait créer un device `/dev/reverse` qui inverse la chaîne de caractères écrite dedans, et la restitue inversée lors d’un read. Mais c'est tellement mal foutu, j'ai fait une grosse refacto :  

-	les prototypes des fonctions étaient cassés, j’ai refait a ma sauce les `myfd_read` et `myfd_write`  
-	j’ai remarqué que le module utilisait une variable globale `str` sans la verouiller (data race), donc j’ai ajouté un mutex global pour protéger les accès concurrents  
-	j’ai renommé `str` en `buf` et ajouté `buf_len` pour suivre la longueur utile, beaucoup plus propre et clair  
-	j’ai ajouté un `#define DEVNAME "reverse"` pour ne plus avoir de chaînes en dur  
-	j’ai complètement revu `myfd_read` : la logique originale inversait bien la chaîne, mais faisait un `kmalloc` énorme et pas de free, donc j’ai réécrit avec un `kmalloc` juste à la bonne taille, un `for` propre, et un `kfree` à la fin  
-	j’ai ajouté un check sur `!buf_len` pour retourner 0 quand rien à lire, sinon `cat` bloque  
-	j’ai remplacé `strlen()` par `buf_len`, c’est plus sûr et évite de recalculer sur chaque read  
-	j’ai ajouté `mutex_lock`/`unlock` autour du read pour éviter de lire pendant qu’un `write` modifie la mémoire  
-	j’ai revu `myfd_write` : l’ancien faisait un +1 inutile et cassait la terminaison de chaîne, j’ai corrigé en écrivant juste la taille reçue et en ajoutant le '`\0`' à la bonne place  
-	j’ai limité la copie à `PAGE_SIZE - 1` avec `min()` pour éviter de dépasser le buffer  
-	j’ai aussi forcé `*ppos = 0` avant l’écriture, sinon plusieurs writes s’enchaînaient mal  
-	j’ai ajouté `mutex_lock`/`unlock` dans le write aussi, pour être cohérent avec le read  
-	j’ai corrigé les structs : `myfd_fops` devient `const`, ajout de `.llseek = noop_llseek` pour interdire le `seek` (car ca peut poser probleme avec ce genre de device)  
-	j’ai réécrit `miscdevice` sans les déférencements de fou furieux `&(*(&(myfd_device)))` (wtf?)  
-	dans la fonction `init`, j’ai remplacé `return 1` par `return 0`    
-	j’ai ajouté un vrai `misc_deregister()` dans `exit` pour libérer le device  
-	j’ai mis la licence à “GPL”, et une description claire du module  
-	j’ai fini par vérifier avec `checkpatch.pl --strict`, plus aucun warning  
-	ensuite j’ai testé : `echo bonjour > /dev/reverse` puis `cat /dev/reverse` → `ruojnob`, c'est validé par la street ca  
-	pour finir j’ai demandé a mon pote (ChatGPT) de me faire une batterie de test a mettre dans mon Makefile pour autocheck le tout, ce sera utile pendant l'eval  

> [Code source corrigé ici](../project/08)

</details>

<br>

## Assignment 09

> En cours de réalisation


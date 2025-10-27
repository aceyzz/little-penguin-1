obj-m := reverse.o

KDIR := /lib/modules/$(shell uname -r)/build

PWD := $(shell pwd)

all:
	@echo "[*] Compilation du module kernel..."
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	@echo "[*] Nettoyage..."
	$(MAKE) -C $(KDIR) M=$(PWD) clean

test: all
	@echo
	@echo "==================== TEST MODULE /dev/reverse ===================="
	@echo "[1] Réinitialisation des logs kernel"
	@dmesg -C || true
	@echo "[2] Insertion du module..."
	@sudo insmod reverse.ko || { echo "[ERREUR] Insertion échouée"; exit 1; }
	@sleep 1
	@echo "[OK] Module inséré avec succès"
	@echo "[3] Vérification présence du device..."
	@if [ -e /dev/reverse ]; then echo "[OK] /dev/reverse existe"; else echo "[ERREUR] /dev/reverse manquant"; sudo rmmod reverse; exit 1; fi
	@echo
	@echo "---- Test #1 : Chaîne simple ----"
	@echo "[Action] Écrire 'bonjour42' dans /dev/reverse"
	@echo "bonjour42" | sudo tee /dev/reverse >/dev/null
	@echo "[Attendu] 24ruojnob"
	@echo -n "[Obtenu ] "
	@cat /dev/reverse | tr -d '\n'
	@sleep 1
	@echo
	@echo
	@echo "---- Test #2 : Chaîne vide ----"
	@echo "[Action] Écrire une chaîne vide"
	@echo -n "" | sudo tee /dev/reverse >/dev/null
	@echo "[Attendu] résultat du dernier test (Test #1)"
	@echo -n "[Obtenu ] "
	@cat /dev/reverse | tr -d '\n' || true
	@sleep 1
	@echo
	@echo
	@echo "---- Test #3 : Caractères spéciaux ----"
	@echo "[Action] Écrire '!@#123abc' dans /dev/reverse"
	@echo '!@#123abc' | sudo tee /dev/reverse >/dev/null
	@echo "[Attendu] cba321#@!"
	@echo -n "[Obtenu ] "
	@cat /dev/reverse | tr -d '\n'
	@sleep 1
	@echo
	@echo
	@echo "[5] Logs du kernel récents :"
	@dmesg | tail -n 10
	@echo "[6] Suppression du module..."
	@sudo rmmod reverse
	@echo "[OK] Module retiré"
	@echo "================================================================="

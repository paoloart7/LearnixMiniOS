# 🖥️ LearnixMiniOS

[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)]()

Un système d'exploitation éducatif moderne écrit en C et Assembleur x86, implémentant des concepts d'OS.

## 🚀 Fonctionnalités

### ✅ Implémentées
- **Bootloader** 16/32 bits avec transition en mode protégé
- **Kernel monolithique** en C/ASM
- **Gestionnaire d'interruptions** (IDT, PIC)
- **Pilote clavier** PS/2 basique
- **Système vidéo** VGA texte (80x25) avec couleurs
- **Gestionnaire de temps** (timer PIT)
- **Système de processus** basique
- **Ordonnanceur** round-robin
- **Interface utilisateur** textuelle

## 🛠️ Installation

### Prérequis
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential nasm qemu-system-x86 gdb-multiarch

# Compilateur croisé i686-elf (requis)
# Suivre: https://wiki.osdev.org/GCC_Cross-Compiler
Compilation
bash
# Cloner le dépôt
git clone https://github.com/paoloart7/LearnixMiniOS.git
cd LearnixMiniOS

# Compiler l'OS
make os

# Alternative avec le script
./scripts/build.sh

Utilisation Commandes principales
bash
# Compiler et exécuter
make run

# Déboguer avec GDB (2 terminaux)
make debug      # Terminal 1: Lance QEMU en mode debug
make gdb        # Terminal 2: Connecte GDB automatiquement

# Compiler uniquement le kernel
make kernel

# Nettoyer
make clean      # Nettoyer les builds
make clean-all  # Nettoyer complètement
Débogage avancé
bash
# Mode debug simple (sans auto-continue)
make gdb-simple

# Debug avec informations détaillées
make debug-full

# Voir toutes les commandes disponibles
make help
🔧 Détails Techniques
Architecture
CPU : x86 32-bit (i686)

Mode : Protected Mode

Mémoire : Kernel à 0x100000 (1MB), Heap/Stack dédiés

Interruptions : IDT avec 256 entrées, PIC remappé

Temporel : PIT à 100Hz (10ms par tick)

Adresses Mémoire
0x7C00 : Bootloader

0x100000 : Kernel (1MB)

0xB8000 : Mémoire vidéo VGA

0x0000-0xFFFF : Tables système (GDT, IDT)

Pilotes
VGA : Mode texte 80x25, 16 couleurs

Clavier : PS/2, buffer circulaire

Timer : PIT (8253/8254), interruptions IRQ0

PIC : 8259A, IRQ0-7 remappés à 0x20-0x27

📚 Documentation
Flux de Démarrage
BIOS charge le secteur 1 à 0x7C00

Bootloader initialise GDT, passe en mode protégé

Kernel chargé à 0x100000, IDT initialisée

Pilotes initialisés (VGA, clavier, timer)

Ordonnanceur démarré, premier processus créé

Interface utilisateur affichée

🧪 Tests
Scénarios de test validés
bash
# Test 1: Compilation complète
make clean && make os
# ✅ Réussi si: Pas d'erreurs, bin/os.bin créé

# Test 2: Démarrage système
make run
# ✅ Réussi si: Interface système s'affiche

Implémenter les fonctions d'initialisation et d'IRQ

Ajouter au kernel.c dans kernel_main()

Tester avec make run et make gdb-simple

🤝 Contribution
Fork le projet

Créer une branche : git checkout -b feature/nouvelle-fonctionnalite

Commit : git commit -m 'Ajout: description'

Push : git push origin feature/nouvelle-fonctionnalite

Pull Request

📝 Licence
Ce projet n'est sous aucune licence .

🙏 Remerciements
OSDev Wiki pour l'excellente documentation
Intel pour les manuels d'architecture
QEMU pour l'émulation de qualité
Communauté OSDev pour le support
Professeur Jean Andris Adam pour l'encadrement académique

📚 Bibliographie
Intel Corporation, *Intel® 64 and IA-32 Architectures Software Developer's Manual*, 2021.
OSDev Community, "Bare Bones", OSDev Wiki, 2023.
OSDev Community, "IDT Tutorial", OSDev Wiki, 2023.
Duntemann, J., Assembly Language Step-by-Step, Wiley, 2009.
Tanenbaum, A., Modern Operating Systems, Pearson, 2014.

👤 Auteur
Karlsen PAUL
Étudiant en Genie Informatique a l'Universite Quisqueya
📧 karlsen.paul@uniq.edu
🐙 paoloart7
🔗 https://github.com/paoloart7/LearnixMiniOS.git

📅 Version
Version actuelle : 1.0.0
Dernière mise à jour : 2026-01-30
Statut : En développement



# CellSight Lite — kit étudiant

Application de bureau C++/Qt pour explorer les données de cyclage de cellules
lithium-ion du laboratoire : fiche de chaque cellule, courbes de mesures,
indicateurs (capacité, SOH, résistance interne, SOC...), exports CSV et MATLAB.

CellSight Lite est la **partie C++** du projet **CellSight** du laboratoire.
Le sujet complet (`Projet_CellSight.pdf`) décrit le contexte, l'architecture et
les étapes, une par une.

## Démarrer

1. Installer **Qt 6** (Qt Online Installer, version LTS) avec les composants
   *MinGW 64-bit*, *Qt Charts*, *CMake* et *Ninja*.
2. Dans Qt Creator : **Fichier > Ouvrir un fichier ou projet…** et choisir
   `CMakeLists.txt` à la racine de ce dossier.
3. Compiler, puis lancer la cible **`cellsight_tests`** : elle affiche le bilan
   des étapes (`OK`, `ECHEC` ou `A FAIRE`).
4. Lancer la cible **`cellsight_gui`** : l'application fonctionne dès le départ
   et s'enrichit à chaque étape terminée.

Sans Qt, la bibliothèque et les tests se compilent seuls :

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build -R etape3 --output-on-failure   # une seule étape
```

## Organisation

```
core/include/cellsight/   les en-têtes : toutes les classes sont déclarées
core/src/                 les implémentations : les « TODO Étape N » sont pour vous
tests/                    un fichier de tests par étape (ne pas modifier)
gui/                      l'interface Qt (étape 8 : quelques trous à compléter)
data/                     données SYNTHÉTIQUES du laboratoire (fabricants fictifs)
```

Chercher `TODO Étape` dans le projet (Ctrl+Shift+F dans Qt Creator) pour
trouver tout ce qui reste à écrire.

## Règles du jeu

- Travail en **binôme** ; les deux membres doivent pouvoir expliquer tout le code.
- Ne pas modifier les tests ni les en-têtes, sauf pour les bonus.
- Une étape est terminée quand `ctest -R etapeN` passe.

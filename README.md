# Cube 3D - Vulkan

## Prérequis & Versions

| Composant | Version |
| :--- | :--- |
| **OS** | Windows 10 (x64) |
| **Langage** | C++20 |
| **Vulkan SDK** | 1.4.357.0 |
| **Include/Libs** | GLFW , GLM, `stb_image.h` |
| **Visual Studio recommandé** | 2026 |

## Compilation & Exécution

### 1. Compilation C++
1. Installez **Vulkan SDK 1.4.357.0** à la racine de votre Windows.
2. Ouvrez la solution `.sln` dans Visual Studio 2026.
3. Compilez la solution.

### 2. Compilation des Shaders (Slang vers SPIR-V)
Les shaders sont déjà compilés `slan.spv`.
Si besoin un script de compilation `compile.bat` est fourni dans le dossier `shaders/`.

### Exécution
Pas besoin de compiler le projet ! Vous pouvez simplement télécharger l'exécutable directement depuis l'onglet Releases du dépôt Git.


## Fonctionnalités et limites du projet

Voir le fichier `Checklist_Recette.md`

## Références

- **Tutoriel Khronos Vulkan :** (https://vulkan-tutorial.com/)
- **Bibliothèques :**
  - [GLFW](https://www.glfw.org/)
  - [GLM](https://github.com/g-truc/glm)
  - [stb_image](https://github.com/nothings/stb)

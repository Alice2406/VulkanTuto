# Note d'Architecture Synthétique - Renderer Vulkan 3D

## 1. Schéma des Dépendances

```text
             [ MainClient.cpp ]     [ VulkanTuto.cpp ]
                   \                        /
                    \                      /
                     v                    v
           +----------------------------------+
           |        Application Cliente       |
           +----------------------------------+
           | - Boucle de jeu                  |
           | - Gestion des entrées (GLFW)     |
           +----------------------------------+
                            |
                            v
           +----------------------------------+
           |      Module Renderer (Core)      |
           +----------------------------------+
           | - VulkanInstance / VulkanDevice  |
           | - VulkanSwapChain / VulkanDepth  |
           | - VulkanPipeline                 |
           | - VulkanVertex / VulkanTextures  |
           | - VulkanDescriptor / VulkanDraw  |
           +----------------------------------+
                            |
                            v
           +----------------------------------+
           |     Vulkan SDK / GPU             |
           +----------------------------------+
```


## 2. Découpage des Responsabilités

| Classe | Responsabilité Principale |
| :--- | :--- |
| `VulkanInstance` / `VulkanDevice` | Initialisation de l'instance, des layers, sélection du GPU physique et création du logical device. |
| `VulkanSwapChain` | Gestion des images, du format de couleur, des Image Views et de la recréation lors du redimensionnement. |
| `VulkanDepth` | Gestion automatique de l'image de profondeur adaptée à la taille de la swapchain. |
| `VulkanPipeline` | Chargement des modules shaders SPIR-V et création du pipeline. |
| `VulkanVertex` | Allocation des Vertex & Index Buffers, Uniform Buffers et Staging Buffers. |
| `VulkanTextures` | Chargement des textures avec `stb_image`, gestion des ressources `vk::Image`, des transitions de layout. |
| `VulkanDescriptor` | Pool de descripteurs et liaisons des UBOs et Samplers vers les shaders. |
| `VulkanDraw` | Synchronisation CPU/GPU avec des fences, enregistrement des `CommandBuffers` et soumission des frames à la queue graphique. |


## 3. Gestion des Ressources et Propriété

1. **Sécurité Mémoire via `vk::raii` :**
2. **Synchronisation CPU/GPU :**
   Avant toute fermeture ou recréation de la swapchain, le renderer exécute un appel à `device.waitIdle()`.

---

## 4. Préparation à l'Intégration au Futur Moteur 3D

Ce module de rendu pose de bonnes bases pour le futur moteur 3D, mais le code actuel n'est malheureusement pas parfait et méritera quelques refactoring et modifications avant d'être intégré définitivement.

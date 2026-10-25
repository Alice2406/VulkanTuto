# Checklist de Recette

## Tableau de Recette

| Vérification | Résultat Attendu | Statut | Observations |
| :--- | :--- | :---: | :--- |
| **Clonage dans un autre dossier** | Compilation possible en suivant le `README.md`, sans chemin absolu propre au poste. | **VALIDÉ** | Chemins de projet configurés en relatif. |
| **Lancement** | Géométrie texturée correctement affichée à l'écran. | **SEMI-VALIDÉ** | Le cube tourne correctement avec la texture appliquée sur chaque face (pas toujours dans le bon sens). |
| **Transformation** | Modification de la transformation depuis l'application cliente sans changement du renderer. | **NON VALIDÉ** | La matrice modèle UBO doit être mise à jour depuis sa class `VulkanDraw`. |
| **Profondeur ** | Occlusion correcte après inversion de l'ordre de dessin des surfaces opaques. | **VALIDÉ** | Le cube tourne sans que toutes les faces soient affichées en permanence. |
| **Redimensionnements successifs** | Affichage et projection adaptés, sans erreur de validation. | **VALIDÉ** | La Swapchain est recréée quand on étire la fenêtre. |
| **Réduction puis restauration** | Reprise correcte du rendu sans boucle active ou crash. | **VALIDÉ** | Mise en pause du rendu lorsque la fenêtre est minimisée. |
| **Texture absente** | Message d'erreur explicite et arrêt maîtrisé. | **VALIDÉ** | Une erreur est renvoyée si le fichier image ne peut pas être chargé par `stb_image`. |
| **Fermeture** | Libération correcte des ressources sans fuite ni crash. | **VALIDÉ** | Un simple `waitIdle()` global suffit pour terminer, puis le RAII nettoie toutes les ressources de lui-même. |
| **Second programme client** | Réutilisation du même module sans modification de ses sources. | **VALIDÉ** | Le second exécutable (`Triangle.exe`) réutilise le renderer avec un simple triangle. |

## Limites constatées

- Les shaders SPIR-V (.spv) ne se compilent pas automatiquement lors du build : il faut les régénérer à la main en cas de modification.

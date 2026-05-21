function switchTab(tabName) {
    // 1. Masquer tous les blocs de contenu
    const contents = document.querySelectorAll('.tab-content');
    contents.forEach(content => content.classList.remove('active'));

    // 2. Retirer l'état "actif" de tous les boutons de la navbar
    const buttons = document.querySelectorAll('.tab-btn');
    buttons.forEach(button => button.classList.remove('active'));

    // 3. Afficher le bloc demandé
    const targetContent = document.getElementById('tab-' + tabName);
    if (targetContent) {
        targetContent.classList.add('active');
    }

    // 4. Mettre en surbrillance le bouton cliqué
    if (event && event.currentTarget) {
        event.currentTarget.classList.add('active');
    }
}
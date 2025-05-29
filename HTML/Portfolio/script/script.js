function toggleDarkMode() {
    let element = document.body;
    element.classList.toggle("dark-mode");

    const isCurrentlyDarkMode = element.classList.contains("dark-mode");
    localStorage.setItem(
      "darkMode",
      isCurrentlyDarkMode ? "enabled" : "disabled"
    );
}
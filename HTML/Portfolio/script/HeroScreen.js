let body = document.querySelector('.verticalLayout')


function createHeroScreen() {
    const profile = 
    `
        <div class="profile">
          <h1 class="name">Juan Raymond</h1>
          <img class="photo" src="resources/Photo.png" alt="" />
        </div>
        <h2 class="headline">
          Hi I'm a Software Developer with specialized skill in frontend
          accumulated from wide range of industry from game development,
          database, automotive, and animation.
        </h2>
        <h2 class="headline">Welcome to my internet space.</h2>
        <h2 class="headline">
          Click on the hamburger icon at the top left to navigate.
        </h2>
      </div>
    `
    body.innerHTML += profile;
}

createHeroScreen()
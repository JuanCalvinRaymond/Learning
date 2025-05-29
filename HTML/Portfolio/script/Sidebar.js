const pages = [
    {
        header: 'Home',
        link: '#'
    },
    {
        header: 'Skill',
        link: '#skill' 
    },
    {
        header: 'Experience',
        link: '#experience' 
    },
    {
        header: 'Projects',
        link: '#portfolio'
    },
    {
        header: 'Blog',
        link: ''
    }

]

let sidebarElement = document.querySelector('.sidebar');
let opened;

function createNavigationButton() {
    pages.forEach((data)=> {
        
        const button =
            `
                <button onclick="document.location=\'${data.link}\'">${data.header}</button>
            ` 
        sidebarElement.innerHTML += button;
    })
}

function toggleNav() {
    opened ? sidebarElement.style.width = "0px" : sidebarElement.style.width = "250px";
    opened = !opened;
}


createNavigationButton();
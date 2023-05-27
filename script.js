var opened = false;

/* Set the width of the side navigation to 250px and the left margin of the page content to 250px */

function openNav() {
  document.getElementById("mySidenav").style.width = "250px";
  document.getElementById("main").style.opacity = .5;
  if(opened) {
    closeNav();
  }
  else {
    opened = true;
    document.getElementById("menuicon").style.transform = "rotate(90deg)";
  }
} 

function closeNav() {
  document.getElementById("mySidenav").style.width = "0";
  document.getElementById("main").style.opacity = 1;
}
/* Set the width of the side navigation to 0 and the left margin of the page content to 0 */

function closeNav() {
  document.getElementById("mySidenav").style.width = "0";
  opened = false;
  document.getElementById("main").style.opacity = 1;
  document.getElementById("menuicon").style.transform = "rotate(0deg)";
}
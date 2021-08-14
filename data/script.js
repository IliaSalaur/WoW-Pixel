window.addEventListener('load', getNets);
//window.setInterval(getNets(), 10000);
function onClick(id)
{            
    document.getElementById("ssid1").value = document.getElementById(id).innerHTML;
}
function getNets(){            
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
        var myObj = JSON.parse(this.responseText);
        console.log(myObj);
        for(let i = 0; i < 10; i++)
        {
            document.getElementById(i).innerHTML = " ";
        }
        for (let index = 0; index < Object.keys(myObj).length; index++) {
            document.getElementById(index).innerHTML = myObj[index];   
            document.getElementById(index).setAttribute("display", "block");                    
            }
        }
    }; 
    xhr.open("GET", "/nets", true);
    xhr.send();     
    console.log("func");                
}
setInterval(getNets, 10000);
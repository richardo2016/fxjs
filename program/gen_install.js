var fs = require("fs");
var process = require("process");

var build = false;

if (!fs.exists("fxjs.cab"))
	build = true;
else {
	var st1 = fs.stat("fxjs.exe");
	var st2 = fs.stat("fxjs.cab");
	build = st2.mtime < st1.mtime;
}

if (build)
	process.run("makecab", ["fxjs.exe", "fxjs.cab"]);

var txt = JSON.stringify(fs.openFile("fxjs.cab").readAll().toArray());
var src = "unsigned char js_data[] = {" + txt.substr(1, txt.length - 2) + "};\n";
if (!fs.exists("js.h") || fs.readFile("js.h") != src)
	fs.writeFile("js.h", src);
// Imports
const express = require('express')
const app = express()
const port = 5000

// папка с готовым gui после webpack'a
const distDir = __dirname + "/../../data";

// путь до css
app.use(express.static(distDir));

// Navigation
app.get('/', (req, res) => {
    res.sendFile("index.html", {root : distDir });
})

//Start server
app.listen(port, "0.0.0.0", function () {
    console.log("Cek-iot local server started on " + port +  " port!");
});
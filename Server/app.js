const express = require('express');
const mysql = require('mysql2');

const app = express();
const PORT = 3000;


function checkDB(id, res) {
    con.query("SELECT * FROM privileges WHERE id = \'" + id + '\'', function (err, result, fields) {
        if (result.length >= 1){
            privilege = result[0]['privilege'];
            res.send(`permission:${privilege}`)
        }
        else{
            res.send(`permission:${-1}`)
        }
    });
}

// Define route for handling incoming requests
app.get('/:id', (req, res) => {
    const id = req.params.id;
    
    // Perform checks based on the ID
    const result = checkDB(id, res);
});

// Connect to SQL database
var con = mysql.createConnection({
    host: "localhost",
    user: "server",
    password: "Ft9chxvxW7",
    database: 'access_privileges'
});
con.connect(function(err) {
    if (err) throw err;
    console.log("Connected!");
});
  

// Start the server
app.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`);
});



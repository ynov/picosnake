#include "page.h"

const char* PageContent = R"(<!DOCTYPE html>
<html>
<head>

<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>PicoSnake</title>
<style>
* {
    font-family: monospace;
    box-sizing: border-box;
}

body {
    display: flex;
    justify-content: center;
    align-items: center;
    height: 100vh;
    padding: 0;
    margin: 0;
    background-color: #a9a9a9;
}

.container {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    text-align: center;
    width: 100%;
    max-width: 640px;
    background-color: #f0f0f0;
}

.button {
    display: block;
    border: 1px solid #ccc;
    padding: 62px 32px;
    font-size: 24px;
    width: 100%;
}

.button:hover {
    background-color: #ccc;
    cursor: pointer;
}

.button:active {
    background-color: #ccc;
}

.row {
    display: flex;
    flex-direction: row;
    justify-content: center;
    width: 100%;
    flex: 1;
}
</style>

</head>
<body>

<div class="container">
    <div class="row">
        <button type="button" class="button" onclick="window.cmd.up();">UP</button>
    </div>
    <div class="row">
        <button type="button" class="button middle" onclick="window.cmd.left();">LEFT</button>
        <button type="button" class="button middle" onclick="window.cmd.right();">RIGHT</button>
    </div>
    <div class="row">
        <button type="button" class="button" onclick="window.cmd.down();">DOWN</button>
    </div>
</div>


<script>
window.cmd = {
    up: function() {
        window.fetch('/up', { method: 'GET' });
    },
    left: function() {
        window.fetch('/left', { method: 'GET' });
    },
    right: function() {
        window.fetch('/right', { method: 'GET' });
    },
    down: function() {
        window.fetch('/down', { method: 'GET' });
    }
};
</script>

</body>
</html>
)";

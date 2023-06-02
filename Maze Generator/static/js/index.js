/*
# 
# This file was created as part of CS 340's Spring 2022 Final Project
# - This file was further modified for MP9.
# - It should not need to be modified further for you to complete MP9 -- you need only focus on the MGs! :)
#
*/


// Make the paper scope global, by injecting it into window:
paper.install(window);

var zoomlevel = 200;
var maze = new Maze(zoomlevel); //CANVAS_H = 600 -> 3 blocks high
var colorSelectionModal;
var userColorHex;

// Generate a random user ID for now
const getRandomLetters = (length = 1) => Array(length).fill().map(e => String.fromCharCode(Math.floor(Math.random() * 26) + 65 + 32)).join('');
let uid = localStorage.getItem("cs240_maze_uid");

if (!uid) {
  uid = getRandomLetters(16);
  localStorage.setItem("cs240_maze_uid", uid);
}

// $( function ) runs once the DOM is ready:
$(() => {
  if (typeof oneMaze !== "undefined") {
    userColorHex = "#000000";
    paper.setup("myCanvas");
    requestGrid(-3, -3);
    return;
  }

  let randomColor = localStorage.getItem("cs240_maze_color");
  if (!randomColor) {
    randomColor = "#" + Math.floor(Math.random()*16777215).toString(16);
  }
  $("#colorInput").val(randomColor);

  colorSelectionModal = new bootstrap.Modal(document.getElementById('colorSelectionModal'), {});
  colorSelectionModal.show();
});


initColor = () => {
  colorSelectionModal.hide();

  userColorHex = $("#colorInput").val();
  localStorage.setItem("cs240_maze_color", userColorHex);
  myCrumbs.color = userColorHex;
  $.post(`/addUserColor/${uid}/${userColorHex.substring(1)}`);

  paper.setup("myCanvas");
  requestGrid(-3, -3);

  setTimeout(() => { sendHeartbeat(); }, 1000);
  setTimeout(() => { movePlayers(); }, 1000);  
};


zoomMaze = () => {
  zoomlevel /= 2;
  if (zoomlevel < 20) {
    zoomlevel = 200;
  }
  maze.zoom(zoomlevel);
};

computeUnit = (requestX, requestY) => {
  return {
    col: Math.floor( ((requestX + BLOCK_C) / BLOCK_W) + 0.5 ),
    row: Math.floor( ((requestY + BLOCK_C) / BLOCK_W) + 0.5 ),
  };
};

grid = {};
gridColors = {};
requestX = -3;
requestY = -3;
x = 0;
y = 0;

(minX = 0), (maxX = 0), (minY = 0), (maxY = 0);

requestGrid = (requestX, requestY) => {
  console.log(`RequestGrid(${requestX}, ${requestY})`);

  let gen_seg_request;
  let jsonData = undefined;
  let method = "GET";
  let dataType = undefined;

  if (typeof oneMaze !== "undefined") {
    gen_seg_request = "/generateSegment/" + oneMaze;
    
    let unit = computeUnit(requestX, requestY);
    let y = unit["col"];
    let x = unit["row"];
    jsonData = {
      "main": [ x, y ],
      "free": []
    }

    for (let dx = -10; dx < 10; dx++) {
      for (let dy = -10; dy < 10; dy++) {
        let key = (y + dy) + "," + (x + dx);
        if (!(key in gridColors)) {
          jsonData["free"].push(x + dx);
          jsonData["free"].push(y+ dy);
        }
      }
    }
    method = "POST";
    dataType = "application/json";
    jsonData = JSON.stringify(jsonData);
  } else {
    gen_seg_request = "/" + uid + "/generateSegment";
    jsonData = computeUnit(requestX, requestY);
  }

  $.ajax({
    type: method,
    url: gen_seg_request,
    data: jsonData,
    contentType: dataType
  })
  .done(function (data) {
    // get origin information for the maze segment
    var ox = data["originX"] ?? 0;
    var oy = data["originY"] ?? 0;
    
    // adjust the request's x and y based on segment origin
    let gridUnit = computeUnit(requestX, requestY);
    let ry = (gridUnit.row * BLOCK_W) - 3;
    let rx = (gridUnit.col * BLOCK_W) - 3;
    
    // verify we don't have a multiblock segment with no origin
    let geom = data["geom"];
    if (!(geom.length == BLOCK_W && geom[0].length == BLOCK_W)) {
      if (!("originX" in data && "originY" in data)) {
        alert(
          "WARNING: origin X and Y not specified for multiblock maze segment"
        );
        return false;
      }
    }

    let populateGrid = (rx, ry, geom) => {
      // populate the local grid as necessary
      console.log(`POPULATE: ${rx} ${ry}`)
      for (let curY = 0; curY < geom.length; curY++) {
        let g = geom[curY];

        for (let curX = 0; curX < g.length; curX++) {
          let c = g[curX];

          if (!grid[curX + rx]) {
            grid[curX + rx] = {};
          }
          grid[rx + curX][ry + curY] = c;

          if (rx + curX < minX) {
            minX = rx + curX;
          }
          if (rx + curX > maxX) {
            maxX = rx + curX;
          }
          if (ry + curY < minY) {
            minY = ry + curY;
          }
          if (ry + curY > maxY) {
            maxY = ry + curY;
          }
        }
      }
    };
    populateGrid(rx, ry, geom);
    
    if (data["extern"]) {
      for (let key in data["extern"]) {
        let gridCoord = key.split("_");
        let gY = +gridCoord[0];
        let gX = +gridCoord[1];
        let gGeom = data["extern"][key]["geom"];


        let absX = -3 + (gX * 7);
        let absY = -3 + (gY * 7);

        console.log(`ADDING: ${gX},${gY} => ${absX},${absY}`);

        let gridString = `${gX},${gY}`
        gridColors[gridString] = userColorHex;

        if (!(absX == rx && absY == ry)) {
          populateGrid(absX, absY, gGeom);
          maze.addBlock(absX, absY, gGeom);
        }


      }
    }

    console.log(grid);

    let gridString = gridUnit["col"] + "," + gridUnit["row"];
    if ("color" in data) { // If color is passed through with data (if block has already been generated)
      gridColors[gridString] = data["color"]
    } else { // If this is the first time the block is being generated
      gridColors[gridString] = userColorHex
    }

    // actually add the block to the grid for rendering purposes
    maze.addBlock(rx, ry, geom);
  })
  .fail(function (data) {
    let responseText = data["responseText"];
    $("#maze").html(`<hr><h2>${responseText}</h2><hr><div style="text-align: left"><h4>Response Data</h4><pre style="color: red;">${JSON.stringify(data, undefined, "&nbsp;")}</pre>`);    
  });
};

expandGrid = (dX, dY) => {
  if (dX == 1) {
    requestGrid(x, y - 3);
  }
  if (dX == -1) {
    requestGrid(x - 6, y - 3);
  }
  if (dY == 1) {
    requestGrid(x - 3, y);
  }
  if (dY == -1) {
    requestGrid(x - 3, y - 6);
  }
};

move = (dX, dY) => {
  if (!grid[x] || !grid[x][y]) {
    return false; //ignore key events if our current maze section isn't loaded
  }

  x += dX;
  y += dY;

  if (!grid[x] || !grid[x][y]) {
    console.log("Expand Grid!");
    expandGrid(dX, dY);
  }

  maze.renderPlayer(x, y);
  maze.renderMaze();
};

const NORTH_WALL_MASK = 8;
const EAST_WALL_MASK = 4;
const SOUTH_WALL_MASK = 2;
const WEST_WALL_MASK = 1;

let forceMoveTimer = 10;
forceMoveUp = () => {
  forceMoveTimer = 10;
  $("#stuck").prop("disabled", true);
  $("#stuck").html("Forcing Movement -- Wait 10s...");
  setTimeout(forceMoveCallback, 1000);
};

forceMoveCallback = () => {
  forceMoveTimer--;

  if (forceMoveTimer > 0) {
    $("#stuck").html(`Forcing Movement -- Wait ${forceMoveTimer}s...`);
    setTimeout(forceMoveCallback, 1000);
  } else {
    $("#stuck").prop("disabled", false);
    $("#stuck").html("Force Move Up One Square (Takes 10s)");
    myCrumbs["steps"].push([x, y]);
    move(0, -1);
  }
}

document.onkeydown = (e) => {
  let sq = parseInt(grid[x][y], 16);
  let wallNorth = sq & NORTH_WALL_MASK;
  let wallEast = sq & EAST_WALL_MASK;
  let wallSouth = sq & SOUTH_WALL_MASK;
  let wallWest = sq & WEST_WALL_MASK;

  if (e.keyCode == "38" && !wallNorth) {
    if (grid[x] && grid[x][y - 1] && parseInt(grid[x][y - 1], 16) & SOUTH_WALL_MASK) { return; }
    myCrumbs["steps"].push([x, y]);
    move(0, -1);
  } else if (e.keyCode == "40" && !wallSouth) {
    if (grid[x] && grid[x][y + 1] && parseInt(grid[x][y + 1], 16) & NORTH_WALL_MASK) { return; }
    myCrumbs["steps"].push([x, y]);
    move(0, 1);
  } else if (e.keyCode == "37" && !wallWest) {
    if (grid[x - 1] && grid[x - 1][y] && parseInt(grid[x - 1][y], 16) & EAST_WALL_MASK) { return; }
    myCrumbs["steps"].push([x, y]);
    move(-1, 0);
  } else if (e.keyCode == "39" && !wallEast) {
    if (grid[x + 1] && grid[x + 1][y] && parseInt(grid[x + 1][y], 16) & WEST_WALL_MASK) { return; }
    myCrumbs["steps"].push([x, y]);
    move(1, 0);
  } else if (e.keyCode == "90") {
    zoomMaze();
  } else if (e.keyCode == '32') {
    myCrumbs["steps"] = [];
    x = 0; y = 0;
    maze.renderPlayer(x, y);
  }
};

// player breadcrumb update heartbeat
const myCrumbs = {
  "user"  : uid,
  "x"     : x,
  "y"     : y,
  "steps" : [],
  "color" : undefined,
};

var players = {};

movePlayers = () => {
  maze.renderMaze();
  // document.getElementById("debug").innerHTML = JSON.stringify(players);

  setTimeout(() => {
    movePlayers();
  }, 100);
}

sendHeartbeat = () => {
  // Update my crumbs:
  myCrumbs.x = x;
  myCrumbs.y = y;  
  
  // Create a summary for the server:
  let serverCrumbs = {
    user: myCrumbs.user,
    x: myCrumbs.x,
    y: myCrumbs.y,
    steps: myCrumbs.steps.slice(-10),
    color: myCrumbs.color,
  }
  serverCrumbs.steps.reverse();

  $.ajax({
    type: "PATCH",
    url: "/heartbeat",
    data: JSON.stringify(serverCrumbs),
    contentType: "application/json; charset=utf-8",
  })
  .done(function (data) {
    for (const [k, p] of Object.entries(data)) {
      if (k == "_totalBlocks") {
        $("#total-segments").html(`Total Maze Blocks: ${p}`);
        continue;
      } else if (k == "_userBlocks") {
        $("#user-segments").html(`Maze Blocks <b>YOU</b> Discovered: ${p}`);
      }

      if (!(k in players)) {
        players[k] = p;
      }
      else if (p["time"] > players[k]["time"]) {
        players[k] = p;
      }
    }
  });

  setTimeout(() => {
    sendHeartbeat();
  }, 1000);
}

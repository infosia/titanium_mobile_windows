"use strict";

var CELL_SIZE = 10,
	FPS_INTERVAL = 30;

var Window = require('Windows.UI.Xaml.Window'),
    Colors = require('Windows.UI.Colors'),
    window = Window.Current,
    SolidColorBrush = require('Windows.UI.Xaml.Media.SolidColorBrush'),
    Canvas = require('Windows.UI.Xaml.Controls.Canvas'),
    TextBlock = require('Windows.UI.Xaml.Controls.TextBlock'),
	White = new SolidColorBrush(Colors.White),
	Black = new SolidColorBrush(Colors.Black),
	universe = new Canvas(),
	label = new TextBlock(),
    Visibility = Windows.UI.Xaml.Visibility;

// set up universe
universe.Height = window.Bounds.Height;
universe.Width = window.Bounds.Width;
universe.Background = Black;

// set up label
label.Text = 'Loading, please wait.';
label.TextAlignment = Windows.UI.Xaml.TextAlignment.Right;
label.VerticalAlignment = Windows.UI.Xaml.VerticalAlignment.Top;
label.HorizontalAlignment = Windows.UI.Xaml.HorizontalAlignment.Right;
label.FontSize = 60;
universe.Children.Append(label);

// set up the rest of the variables
var xSize = (universe.Width / CELL_SIZE) | 0,
	ySize = (universe.Height / CELL_SIZE) | 0;

// populate cells
Ti.API.info('populating cells...');
var cells = [];
for (var x = 0; x < xSize; x++) {
    cells[x] = [];
    for (var y = 0; y < ySize; y++) {
        // is this cell alive?
        var alive = Math.random() >= 0.5;

        // create native UI view
        var cellProxy = new Canvas();
        cellProxy.Height = CELL_SIZE;
        cellProxy.Width = CELL_SIZE;
        Canvas.SetLeft(cellProxy, x * CELL_SIZE);
        Canvas.SetTop(cellProxy, y * CELL_SIZE);
        cellProxy.Background = White;
        cellProxy.Visibility = alive ? Visibility.Visible : Visibility.Collapsed;

        // save the cell
        cells[x][y] = {
            proxy: cellProxy,
            lastAlive: alive,
            alive: alive
        };

        // add cell to universe
        universe.Children.Append(cellProxy);
    }
}
Ti.API.info('done populating cells.');

// start app
window.Content = universe;
window.Activate();

// the render function
var ctr = 0;
var lastReport = Date.now();

function update(params) {
    var x, y, cell;

    // render current generation
    for (x = 0; x < xSize; x++) {
        for (y = 0; y < ySize; y++) {
            cell = cells[x][y];

            // minimize number of times we need to modify the proxy object
            if (cell.alive !== cell.lastAlive) {
                cell.proxy.Visibility = cell.alive ? Visibility.Visible : Visibility.Collapsed;
                //cell.proxy.setHidden(!cell.alive);
            }

            // save the state
            cell.lastAlive = cell.alive;
        }
    }

    // build next generation
    for (x = 0; x < xSize; x++) {
        for (y = 0; y < ySize; y++) {
            cell = cells[x][y];
            cell.alive = getNextState(x, y, cell.lastAlive);
        }
    }

    // show the average FPS
    if (!(++ctr % FPS_INTERVAL)) {
        ctr = 1;
        var currentReport = Date.now(),
			fps = 1.0 / (currentReport - lastReport);
        label.Text = 'FPS: ' + Math.round(FPS_INTERVAL * 10 * fps) / 10;
        lastReport = currentReport;
    }
}

// calculate the next state of each cell
function getNextState(x, y, alive) {
    var count = 0,
		xm1 = x > 0,
		xp1 = x + 1 < xSize,
		ym1 = y > 0,
		yp1 = y + 1 < ySize;

    if (xm1) {
        if (ym1 && cells[x - 1][y - 1].lastAlive) { count++; }
        if (cells[x - 1][y].lastAlive) { count++; }
        if (yp1 && cells[x - 1][y + 1].lastAlive) { count++; }
    }
    if (xp1) {
        if (ym1 && cells[x + 1][y - 1].lastAlive) { count++; }
        if (cells[x + 1][y].lastAlive) { count++; }
        if (yp1 && cells[x + 1][y + 1].lastAlive) { count++; }
    }
    if (ym1 && cells[x][y - 1].lastAlive) { count++; }
    if (yp1 && cells[x][y + 1].lastAlive) { count++; }

    return (alive && (count === 2 || count === 3)) || (!alive && count === 3);
}

setInterval(update, 1000 / FPS_INTERVAL);
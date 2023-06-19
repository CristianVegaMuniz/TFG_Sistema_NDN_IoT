const { app, BrowserWindow } = require('electron')
const path = require('path');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline')
const ndn = require('ndn-js');

var face = new ndn.Face({host:"192.168.1.143"});	// conectarse a nfd

async function onData(interest, data) {
  var values = data.getContent().buf().toString().split("*")
  var v1 = parseFloat(values[0])
  var v2 = parseFloat(values[1])
  console.log('\nValores recibidos por ndn-js:\nTemperatura:', v1);
  console.log('Humedad:', v2);

  await sleep(1000);
  face.expressInterest(new ndn.Name("/temp"), onData);
}

function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}

face.expressInterest(new ndn.Name("/temp"), onData);

// Se declara todo lo necesario para el puerto Serial
const port = new SerialPort({
    path: '/dev/ttyUSB0',
    baudRate: 9600
  })

const parser = port.pipe(new ReadlineParser({ delimiter: '\n' }))

port.on('open',function() {
    console.log('Serial Port Client NDN is opened.');
    
  });


// Crea la ventana de la aplicación web.

const createWindow = () => {
 
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
        contextIsolation: false,
        nodeIntegration: true,
        nodeIntegrationInWorker: true,
        enableRemoteModule: true
    }
  })
  mainWindow.webContents.openDevTools()
  mainWindow.loadFile('index.html')
  

// Parseo de los datos y envío a la aplicación web para reflejarlos.

  parser.on('data', function (data) {
    if(data.includes("Stats")) {
      var values = data.split("*")
      var interests = parseFloat(values[1])
      var datas= parseFloat(values[2])
      var porcentaje= parseFloat(values[3])
      console.log('\nEstadísticas del consumidor:\nInterests enviados:', interests);
      console.log('Datas recibidos:', datas);
      mainWindow.webContents.send('int',interests);
      mainWindow.webContents.send('dat',datas);
      mainWindow.webContents.send('paq',porcentaje);
    }
    else{
    var values = data.split("*")
    var v1 = parseFloat(values[0])
    var v2 = parseFloat(values[1])
    console.log('\nValores recibidos del consumidor:\nTemperatura:', v1);
    console.log('Humedad:', v2);
    mainWindow.webContents.send('temp',v1);
    mainWindow.webContents.send('hum',v2);
    }
  });

}

app.whenReady().then(() => {
  createWindow()

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit()
})

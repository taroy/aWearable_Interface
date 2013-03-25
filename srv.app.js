/**
 * srv.app.js – Handling HTTP:80 Requests
 * This file is part of noduino (c) 2012 Sebastian Müller <c@semu.mp>
 *
 * @package     noduino
 * @author      Sebastian Müller <c@semu.mp>
 * @license     MIT License – http://www.opensource.org/licenses/mit-license.php 
 * @url         https://github.com/semu/noduino
 */
 
define(['kickstart', 'module', 'path', 'fs'], function (kickstart, module, path, fs) {
  var kickstart = kickstart.withConfig({'name': 'localhost', 'port': 8080, 'path': './'});
  var srv = kickstart.srv();
  
  /**
   * Load file with example snippets
   */
  var fileContents = fs.readFileSync('./examples.snippet'),
    sections = (fileContents + '').split('###'),
    examples = {};
  for (var i = 0; i < sections.length; i++) {
    var tmp = sections[i].split("\n"),
      key = tmp.shift();
    tmp.pop();
    examples[key] = tmp.join("\n");
  }

  /** 
   * Catch request for serving home page
   */
  srv.all('/', function(req, res) {
    res.render('home', {jsApp: 'main', active: 'home', title: 'noduino', 'examples': examples});
  });

  /** 
   * Catch request for Getting Started page
   */
  srv.all('/getting-started.html', function(req, res) {
    res.render('getting-started', {jsApp: 'none', active: 'getting-started', title: 'noduino', 'examples': examples});
  });

  /** 
   * Catch request for serving walkLED example page
   */
  srv.all('/example-walkLED.html', function(req, res) {
    res.render('example-walkLED', {jsApp: 'walkLED', active: 'examples', title: 'noduino', 'examples': examples});
  });
  
  /** 
   * Catch request for serving Display - Ingrid
   */
  srv.all('/display-options.html', function(req, res) {
    res.render('display-options', {jsApp: 'none', active: 'display-options', title: 'noduino', 'examples': examples});
  });
  
  /** 
   * Catch request for serving Lights -Ingrid
   */
  srv.all('/lights-options.html', function(req, res) {
    res.render('lights-options', {jsApp: 'none', active: 'lights-options', title: 'noduino', 'examples': examples});
  });
  
  /** 
   * Catch request for serving Vibration -Ingrid
   */
  srv.all('/vibration-options.html', function(req, res) {
    res.render('vibration-options', {jsApp: 'none', active: 'vibration-options', title: 'noduino', 'examples': examples});
  });
  /** 
   * Catch request for serving Sound - Ingrid
   */
  srv.all('/sound-options.html', function(req, res) {
    res.render('sound-options', {jsApp: 'none', active: 'sound-options', title: 'noduino', 'examples': examples});
  });
  
  /** 
   * Catch request for serving savedApp-test - Maria?
   */
  srv.all('/savedApp.html', function(req, res) {
    res.render('savedApp', {jsApp: 'none', active: 'savedApp', title: 'noduino', 'examples': examples});
  });
  
  /** 
  * Catch request for progression tracker/icon test
  */
  srv.all('/progression_icon.html', function(req, res) {
    res.render('progression_icon', {jsApp: 'none', active: 'progression_icon', title: 'noduino', 'examples': examples});
  });
  
  /**
   * Catch request for serving motor example
   */
  srv.all('/example-motor.html', function(req, res) {
    res.render('example-motor', {jsApp: 'motor', active: 'examples', title: 'noduino', 'examples': examples});
  });
  
  /**
   * Catch request for serving request_response page - Ingrid
   */
  srv.all('/request_response.html', function(req, res) {
    res.render('request_response', {jsApp: 'main', active: 'request_response.', title: 'noduino', 'examples': examples});
  });
  
  
  /** 
  * Catch request for progression tracker/icon test
  */
  srv.all('/channel.html', function(req, res) {
    res.render('channel', {jsApp: 'none', active: 'channel', title: 'noduino', 'examples': examples});
  });

  return {'kickstart': kickstart, 'srv': srv};
});
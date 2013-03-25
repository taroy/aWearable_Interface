var p = 'scripts/vendor/';
require(["jquery", p + "dropdown.js", p + "prettify.js", "./scripts/awearable.js"], function($, dd, pf, awearable) {
  $(document).ready(function(e) {
    prettyPrint();
    awearable.bind();
  });
});

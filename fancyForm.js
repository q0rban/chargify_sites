Drupal.behaviors.fancyFormDescriptions = function (context) {
  var inputs = $('div.form-item input', context);
  inputs.parents('div.form-item:first').find('div.description').hide();

  inputs.focus(function () {
    $(this).parents('div.form-item:first').find('div.description').show('fast');
  })
  .blur(function () {
    $(this).parents('div.form-item:first').find('div.description').hide('fast');
  });
};

Drupal.behaviors.fancyFormLabels = function (context) {
  var fields = $('input.form-text', context).not('[type=password]');
  var forms = fields.parents('form:first');

  // We need to bind to both keydown and keyup in case TAB is used.
  fields.blur(fancyBlur).focus(fancyFocus).bind('keydown.autocomplete', showHideLabels).bind('keyup.autocomplete', showHideLabels).blur().each(putLabels).each(showHideLabels);

  forms.each(function() {
    $(this).submit(function() {
      $('input[type=submit]', this).attr('disabled', 'disabled').val("Please wait...");
    });
  });

  function fancyBlur() {
    $(this).siblings().filter('div.fancy-label').children().removeClass('active');
  }

  function fancyFocus() {
    $(this).siblings().filter('div.fancy-label').children().addClass('active');
  }

  // @todo, this needs more logic to handle the first keystroke. Currently 
  // doesn't hide right away like it should.
  function showHideLabels() {
    var span = $(this).siblings().filter('div.fancy-label').children();
    span.hide();
    if ($(this).val().length < 1) {
      span.show();
    }
  }

  function putLabels() {
    var element = $(this);
    var id = element.attr('id');

    if (element.attr('label')) {
      var val = element.attr('label');
    }
    else {
      var label = $('label[for=' + id + ']');
      var val = label.text().split(':');
      val = val[0];
    }

    // Make sure the label actually fits in to the field.
    if ((this.size > val.length) && label) {
      label.hide();
      var newLabel = '<div class="fancy-label"><span>' + val + '</span></div>';
      var fieldStyle = {
        "background": 'none',
        "position": 'absolute',
        "z-index": '1'
      };
      var divStyle = {
        "background": 'none',
        "width": element.css('width'),
        "height": element.css('height'),
        "background-color": element.css('background-color'),
        "padding-top": element.css('padding-top'),
        "padding-right": element.css('padding-right'),
        "padding-bottom": element.css('padding-bottom'),
        "padding-left": element.css('padding-left'),
        "font-size": element.css('font-size'),
        "line-height": element.css('line-height')
      };
      element.css(fieldStyle).parent().css('position', 'relative').append(newLabel).children().filter('div.fancy-label').css(divStyle).children().hide();
    }
  }
};

Drupal.behaviors.registerLoginForm = function (context) {
  // This is very hastily put together. Need to optimize.
  $('#chargify-sites-payment-form div.login-register-wrapper div.fieldset-content, #chargify-sites-payment-form div.payment, #chargify-sites-payment-form input.form-submit').hide();
  
  $('#chargify-sites-payment-form div.register h2.fieldset-title').not('.active').click(
    function () {
      $('#chargify-sites-payment-form div.payment, #chargify-sites-payment-form input.form-submit').slideDown('fast');
      $(this).addClass('active');
      $('#chargify-sites-payment-form div.login h2.fieldset-title').removeClass('active');
      $('#chargify-sites-payment-form div.register div.fieldset-content').slideDown('fast');
      $('#chargify-sites-payment-form div.login div.fieldset-content').slideUp('fast');
    }
  );
  
  $('#chargify-sites-payment-form div.login h2.fieldset-title').not('.active').click(
    function () {
      $('#chargify-sites-payment-form div.payment, #chargify-sites-payment-form input.form-submit').slideDown('fast');
      $(this).addClass('active');
      $('#chargify-sites-payment-form div.register h2.fieldset-title').removeClass('active');
      $('#chargify-sites-payment-form div.login div.fieldset-content').slideDown('fast');
      $('#chargify-sites-payment-form div.register div.fieldset-content').slideUp('fast');
    }
  );
}
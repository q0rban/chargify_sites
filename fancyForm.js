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

  fields.blur(putLabels).focus(clearLabels).blur();
  forms.each(function() {
    $(this).submit(function() {
      $('input[type=submit]', this).attr('disabled', 'disabled').val("Please wait...");
      fields.focus();
    });
  });

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

    if ((element.val() === this.defaultValue) && (this.size > val.length)) {
      if (label) {
        label.hide();
      }
      element.val(val).attr('label', val);
    }
  }

  function clearLabels() {
    var element = $(this);
    if (element.attr('label') == element.val()) {
      element.val(this.defaultValue);
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
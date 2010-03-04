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
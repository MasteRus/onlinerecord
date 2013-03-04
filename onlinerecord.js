restricted_dates = [
//  [1, 1, 'au']
];

Drupal.behaviors.reserv =
  function (context) {
    for (var key in Drupal.settings.onlinerecord.restricted_dates)
        {
            mydate=Drupal.settings.onlinerecord.restricted_dates[key];
            restricted_dates.push([mydate.split('-')[1],mydate.split('-')[2],'te']);
        }
    for (var id in Drupal.settings.datePopup) {
      Drupal.settings.datePopup[id].settings.beforeShowDay =
          function (date){
              for (i = 0; i < restricted_dates.length; i++) {
                if (date.getMonth() == restricted_dates[i][0] - 1
                && date.getDate() == restricted_dates[i][1]) {
                    return [false, restricted_dates[i][2] + '_day'];
                }
              }
              return [true, ''];
          }
    };
  };

//})(jQuery);
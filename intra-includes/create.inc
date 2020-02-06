[% IF whole.error %]
[% IF whole.status == 'missing_identifier' %]
<p><em>Pozor:</em> Povinné pole Identifikátor není vyplněno.</p>
[% ELSIF whole.status == 'missing_branch' %]
<p><em>Pozor:</em> Knihovna je povinné pole.</p>
[% ELSIF whole.status == 'invalid_borrower' %]
<p><em>Pozor:</em> Vložili jste neplatné údaje o čtenáři.</p>
[% ELSIF whole.status == 'invalid_branch' %]
<p><em>Pozor:</em> Vybrali jste neplatnou knihovnu.</p>
[% ELSE %]
<p>>Neznámá chyba</p>
[% END %]
[% END %]

[% IF whole.stage == "form" %]
<h2>Ruční vytvoření MVS požadavku</h2>
<form id="freeform_create_form" method="POST" action=[% here %]>
  <fieldset class="rows">
    <legend>Obecné údaje</legend>
    <ol id="general-freeform-fields">
      <li>
        <label class="required" for="type">Typ požadovaného dokumentu:</label>
        <select name="type" id="type">
          <option value=""/>
          [% IF whole.value.other.type.lower == "book" %]
          <option value="book" selected="selected">Kniha</option>
          [% ELSE %]
          <option value="book">Kniha</option>
          [% END %]
          [% IF whole.value.other.type.lower == "chapter" %]
          <option value="chapter" selected="selected">Kapitola</option>
          [% ELSE %]
          <option value="chapter">Kapitola</option>
          [% END %]
          [% IF whole.value.other.type.lower == "journal" %]
          <option value="journal" selected="selected">Periodikum</option>
          [% ELSE %]
          <option value="journal">Periodikum</option>
          [% END %]
          [% IF whole.value.other.type.lower == "article" %]
          <option value="article" selected="selected">Článek v periodiku</option>
          [% ELSE %]
          <option value="article">Článek v periodiku</option>
          [% END %]
          [% IF whole.value.other.type.lower == "thesis" %]
          <option value="thesis" selected="selected">Závěrečná práce</option>
          [% ELSE %]
          <option value="thesis">Závěrečná práce</option>
          [% END %]
          [% IF whole.value.other.type.lower == "conference" %]
          <option value="conference" selected="selected">Konference</option>
          [% ELSE %]
          <option value="conference">Konference</option>
          [% END %]
          [% IF whole.value.other.type.lower == "other" %]
          <option value="other" selected="selected">Jiný</option>
          [% ELSE %]
          <option value="other">Jiný</option>
          [% END %]
          [% IF whole.value.other.type.lower == "resource" %]
          <option value="resource" selected="selected">Obecný zdroj</option>
          [% ELSE %]
          <option value="resource">Obecný zdroj</option>
          [% END %]
        </select>
      </li>
    </ol>
  </fieldset>
  [% cwd = whole.cwd %]
  [% type = whole.value.other.type %]
  [% IF type %]
      [% INCLUDE "${cwd}/shared-includes/forms/${type}.inc" %]
  [% END %]
  [% INCLUDE "${cwd}/shared-includes/custom_fields.inc" %]
  <fieldset class="rows">
    <legend>Údaje o čtenáři</legend>
    <ol>
      <li>
        <label class="required" for="cardnumber">
          Číslo průkazu, uživatelské jméno nebo příjmení:
        </label>
        <input type="text" name="cardnumber" id="cardnumber" autocomplete="off"
               type="text" value="" />
      </li>
      <li>
        <label class="required" for="branchcode">Vyzvednout v oddělení</label>
        <select id="branchcode" name="branchcode">
          <option value="" />
          [% FOREACH branch IN branches %]
            <option value="[% branch.branchcode %]">
              [% branch.branchname %]
            </option>
          [% END %]
        </select>
      </li>
    </ol>
  </fieldset>
  <fieldset class="action">
    <input id="ill-submit" type="submit" value="Vytvořit" disabled />
    <a class="cancel" href="/cgi-bin/koha/ill/ill-requests.pl">Zrušit</a>
  </fieldset>
  <input type="hidden" name="method" value="create" />
  <input type="hidden" name="stage" value="form" />
  <input type="hidden" name="backend" value="FreeForm" />
</form>
[% BLOCK backend_jsinclude %]
<script type="text/javascript">
    // <![CDATA[]
    [% INCLUDE "${cwd}/shared-includes/shared.js" %]
    // Require a username and branch selection
    document.addEventListener('DOMContentLoaded', function(){
        $('#freeform_create_form #cardnumber, #freeform_create_form #branchcode').change(function() {
            var comp = ['#cardnumber','#branchcode'].filter(function(id) {
                return $(id).val().length > 0;
            });
            $('#ill-submit').attr('disabled', comp.length < 2);
        });
        // Add autocomplete for patron selection
        $('#freeform_create_form #cardnumber').autocomplete({
            source: "/cgi-bin/koha/circ/ysearch.pl",
            minLength: 3,
            select: function( event, ui ) {
                var field = ui.item.cardnumber;
                $('#freeform_create_form #cardnumber').val(field);
                return false;
            }
        })
        .data( "ui-autocomplete" )._renderItem = function( ul, item ) {
            return $( "<li></li>" )
                .data( "ui-autocomplete-item", item )
                .append( "<a>" + item.surname + ", " + item.firstname + " (" + item.cardnumber + ") <small>" + item.address + " " + item.city + " " + item.zipcode + " " + item.country + "</small></a>" )
                .appendTo( ul );
        };
    });
    // ]]>
</script>
[% END %]

[% ELSIF whole.stage == "borrowers" %]
<!-- We need to clarify the borrower that has been requested. -->
<h2>Výběr čtenáře</h2>
<form method="POST" action=[% here %]>
  [% FOREACH prop IN whole.value.other.keys %]
    [% IF prop != 'custom_key' &&  prop != 'custom_value' && prop != 'cardnumber' %]
    <input type="hidden" name="[% prop %]" value="[% whole.value.other.$prop %]">
    [% END %]
  [% END %]
  [% keys = whole.value.other.custom_key.split('\0') %]
  [% values = whole.value.other.custom_value.split('\0') %]
  [% i = 0 %]
  [% FOREACH key IN keys %]
    <input type="hidden" name="custom_key" value="[% key %]">
    <input type="hidden" name="custom_value" value="[% values.$i %]">
  [% i = i + 1 %]
  [% END %]
  <fieldset class="rows">
    <legend>Čtenáři odpovídající příjmení [% surname %]</legend>
    [% FOREACH opt IN whole.value %]
    [% IF opt.key == "brw" %]
    <ol>
      <li>
        <label class="required" for="brw">Čtenář</label>
        <select name="cardnumber" id="cardnumber">
          <option value=""></option>
          [% FOREACH brw IN opt.value %]
          <option value="[% brw.cardnumber %]">
            [% brw.firstname %] [% brw.surname %] ([% brw.cardnumber %])
          </option>
          [% END %]
        </select>
      </li>
    </ol>
    [% END %]
    [% END %]
  </fieldset>
  <fieldset class="action">
    <input type="submit" value="Vybrat"/>
    <a class="cancel" href=[% parent %]>Zrušit</a>
  </fieldset>
</form>

[% ELSE %]
<p>Neznámý stav. Tato situace by neměla nastat.

[% END %]

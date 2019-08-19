[% IF whole.error %]
[% IF whole.status == 'missing_identifier' %]
<p><em>Pozor:</em> Chybí povinné pole Identifikátor.</p>
[% ELSIF whole.status == 'missing_branch' %]
<p><em>Pozor:</em> Knihovna je povinné pole.</p>
[% ELSIF whole.status == 'invalid_borrower' %]
<p><em>Pozor:</em> Vložili jste neplatné údaje o čtenáři.</p>
[% ELSIF whole.status == 'invalid_branch' %]
<p><em>Pozor:</em> Vybrali jste neplatnou knihovnu.</p>
[% ELSE %]
<p>Neznámá chyba</p>
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
        </select>
      </li>
    </ol>
  </fieldset>
  [% cwd = whole.cwd %]
  [% type = whole.value.other.type %]
  [% IF type %]
      [% INCLUDE "${cwd}/shared-includes/forms/${type}.inc" %]
  [% END %]
  <fieldset class="rows">
    <legend>Uživatelská pole</legend>
    <ol id="freeform-fields">
      [% keys = whole.value.other.custom_key.split('\0') %]
      [% values = whole.value.other.custom_value.split('\0') %]
      [% i = 0 %]
      [% FOREACH key IN keys %]
      <li class="form-horizontal">
        <input type="text" class="custom-name" name="custom_key" value="[% key %]"><input type="text" name="custom_value" id="custom-value" value="[% values.$i %]">
        <button value="[% i %]" name="custom_delete" type="submit" class="btn btn-danger btn-sm delete-new-field">
            <span class="fa fa-delete"></span>Smazat
        </button></li>
      </li>
      [% i = i + 1 %]
      [% END %]
    </ol>
    <button type="button" id="add-new-fields" class="btn btn-default">
      <span class="fa fa-plus"></span>
      Přidat nové pole
    </button> 
  </fieldset>
  <fieldset class="rows">
    <legend>Údaje o čtenáři</legend>
    <ol>
      <li>
        <label class="required" for="cardnumber">
          Číslo průkazky, uživatelské jméno nebo příjmení:
        </label>
        <input type="text" name="cardnumber" id="cardnumber"
               type="text" value="" />
      </li>
      <li>
        <label class="required" for="branchcode">Cílová knihovna:</label>
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
    <input type="submit" value="Vytvořit"/>
    <a class="cancel" href="/cgi-bin/koha/ill/ill-requests.pl">Zrušit</a>
  </fieldset>
  <input type="hidden" name="method" value="create" />
  <input type="hidden" name="stage" value="form" />
  <input type="hidden" name="backend" value="FreeForm" />
</form>
[% BLOCK backend_jsinclude %]
<script type="text/javascript">
    // <![CDATA[]
        document.addEventListener('DOMContentLoaded', function() {
            $('#add-new-fields').click(function(e) {
                e.preventDefault();
                var row = '<li class="form-horizontal">' +
                    '<input type="text" class="custom-name" name="custom_key">' +
                    '<input type="text" id="custom-value" name="custom_value"> '+
                    '<button type="button" class="btn btn-danger btn-sm ' +
                    'delete-new-field">' +
                    '<span class="fa fa-delete">' +
                    '</span>Smazat</button></li>';
                $('#freeform-fields').append(row);
            });
            $('#freeform-fields').on('click', '.delete-new-field',
                function(event) {
                    event.preventDefault();
                    $(event.target).parent().remove();
                }
            );
            $('#type').change(function() {
                $('#freeform_create_form').prepend(
                    '<input type="hidden" name="change_type" value="1" />'
                );
                $('#freeform_create_form').submit();
            });
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
    <legend>Tito čtenáři odpovídají příjmení [% surname %]</legend>
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

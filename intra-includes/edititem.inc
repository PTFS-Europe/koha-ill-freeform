[% IF whole.error %]
[% IF whole.status == 'missing_type' %]
<p><em>Pozor:</em> povinné pole Typ požadovaného dokumentu není vyplněno.</p>
[% ELSE %]
<p>Neznámá chyba</p>
[% END %]
[% END %]

<h2>Upravit ručně vložený MVS požadavek</h2>
<form id="freeform_edit_form" method="POST" action=[% here %]>
  <fieldset class="rows">
    <legend>Podrobnosti</legend>
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
          <option value="journal">Journal</option>
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
  <fieldset class="action">
    <input type="submit" value="Aktualizovat"/>
    <a class="cancel" href="/cgi-bin/koha/ill/ill-requests.pl">Zrušit</a>
  </fieldset>
  <input type="hidden" name="illrequest_id" value="[% whole.value.other.illrequest_id %]" />
  <input type="hidden" name="method" value="edititem" />
  <input type="hidden" name="stage" value="form" />
  <input type="hidden" name="backend" value="FreeForm" />
</form>
[% BLOCK backend_jsinclude %]
<script type="text/javascript">
    // <![CDATA[]
    [% INCLUDE "${cwd}/shared-includes/shared.js" %]
    // ]]>
</script>
[% END %]

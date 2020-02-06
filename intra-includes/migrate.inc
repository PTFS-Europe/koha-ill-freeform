[% IF whole.error %]
[% IF whole.status == 'missing_identifier' %]
<p><em>Pozor:</em> povinné pole Identifikátor není vyplněno.</p>
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
<h2>Přesunout MVS požadavek</h2>
<form id="freeform_migrate_form" method="POST" action=[% here %]>
  <fieldset class="rows">
    <legend>Základní informace</legend>
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
          <option value="chapter">Chapter</option>
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
    <legend>Informace o čtenáři</legend>
    <ol>
      <li>
        <label class="required" for="cardnumber">
          Číslo průkazu nebo příjmení:
        </label>
        <input type="text" name="cardnumber" id="cardnumber"
               type="text" value="" />
      </li>
      <li>
        <label class="required" for="branchcode">Destination library:</label>
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
    <input type="submit" value="Přesunout"/>
    <a class="cancel" href="/cgi-bin/koha/ill/ill-requests.pl">Zrušit</a>
  </fieldset>
</form>
[% BLOCK backend_jsinclude %]
<script type="text/javascript">
    // <![CDATA[]
    [% INCLUDE "${cwd}/shared-includes/shared.js" %]
    // ]]>
</script>
[% END %]

[% ELSE %]
<p>Neznámý stav.  K tomuto by nemělo dojít.

[% END %]

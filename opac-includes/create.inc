[% IF whole.error %]
[% IF whole.status == 'missing_title' %]
<p><em>Please Note:</em> Mandatory field Title is missing.</p>
[% ELSIF whole.status == 'missing_identifier' %]
<p><em>Please Note:</em> Mandatory field Identifier is missing.</p>
[% ELSIF whole.status == 'missing_author' %]
<p><em>Please Note:</em> Mandatory field Author is missing.</p>
[% ELSIF whole.status == 'missing_branch' %]
<p><em>Please Note:</em> Branch is a mandatory field.</p>
[% ELSIF whole.status == 'invalid_borrower' %]
<p><em>Please Note:</em> The borrower details you entered are invalid.</p>
[% ELSIF whole.status == 'invalid_branch' %]
<p><em>Please Note:</em> The branch you chose is invalid.</p>
[% ELSE %]
<p>Unhandled error</p>
[% END %]
[% END %]

[% IF whole.stage == "form" %]
<h2>Create a manual ILL request</h2>
<form method="POST" action=[% here %]>
  <fieldset class="rows">
    <legend>Enter details manually</legend>
    <input name="stage" id="stage" value="form" type="hidden"/>
    <input name="method" id="method" value="create" type="hidden"/>
    <input name="backend" id="method" value="FreeForm" type="hidden"/>
    <ol>
      <li>
        <label class="required" for="title">Title:</label>
        <input type="text" name="title" id="title" value="" />
      </li>
      <li>
        <label class="required" for="author">Author:</label>
        <input type="text" name="author" id="author" value="" />
      </li>
      <li>
        <label class="required" for="identifier">Identifier:</label>
        <input type="text" name="identifier" id="identifier" value="" />
      </li>
    </ol>
  </fieldset>
  <fieldset class="rows">
    <legend>Borrower options</legend>
    <ol>
      <li>
        <label class="required" for="branchcode">Destination branch:</label>
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
    <input type="submit" value="Create"/>
    <a class="cancel" href="/cgi-bin/koha/ill/ill-requests.pl">Cancel</a>
  </fieldset>
</form>

[% ELSE %]
<p>Unknown stage.  This should not have happened.

[% END %]

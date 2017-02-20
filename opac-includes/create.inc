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
    <ol id="freeform-fields">
      <li>
        <label class="required" for="title">Title:</label>
        <input type="text" name="title" id="title" value="[% whole.value.other.title %]" />
      </li>
      <li>
        <label class="required" for="author">Author:</label>
        <input type="text" name="author" id="author" value="[% whole.value.other.author %]" />
      </li>
      <li>
        <label class="required" for="identifier">Identifier:</label>
        <input type="text" name="identifier" id="identifier" value="[% whole.value.other.identifier %]" />
      </li>
      [% custom_keys = whole.value.other.custom_key_del.split('\t') %]
      [% custom_values = whole.value.other.custom_value_del.split('\t') %]
      [% i = 0 %]
      [% FOR key IN custom_keys %]
      <li class="form-horizontal">
        <input type="text" class="custom-name" name="custom_key" value="[% key %]">
        <input type="text" name="custom_value" id="custom-value" value="[% custom_values.$i %]">
        <button value="[% i %]" name="custom_delete" type="submit" class="btn btn-danger delete-new-field">
            <span class="fa fa-delete"></span>Delete
        </button></li>
      </li>
      [% i = i + 1 %]
      [% END %]
    <ol>
  </fieldset>
  <button name="add_new_custom" type="submit" id="add-new-fields" class="btn btn-default">
      <span class="fa fa-plus"></span>
      Add new field
  </button> 
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
    <input class="btn btn-default" type="submit" value="Create"/>
    <a class="cancel" href="/cgi-bin/koha/ill/ill-requests.pl">Cancel</a>
  </fieldset>
</form>

[% ELSE %]
<p>Unknown stage.  This should not have happened.

[% END %]
[% BLOCK backend_jsinclude %]
<script type="text/javascript">
    // <![CDATA[]
        $(document).ready(function() {
            $('#add-new-fields').click(function(e) {
                e.preventDefault();
                var row = '<li class="form-horizontal">' +
                    '<input type="text" class="custom-name" name="custom_key">' +
                    '<input type="text" name="custom_value" ' +
                    'id="custom-value" /> '+
                    '<button type="button" class="btn btn-danger ' +
                    'delete-new-field">' +
                    '<span class="fa fa-delete">' +
                    '</span>Delete</button></li>';
                $('#freeform-fields').append(row);
            });
            $('#freeform-fields').on('click', '.delete-new-field',
                function(event) {
                    event.preventDefault();
                    $(event.target).parent().remove();
                }
            );
        });
    // ]]>
</script>
[% END %]

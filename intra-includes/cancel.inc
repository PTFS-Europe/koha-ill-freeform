[% IF whole.error %]
<p>Neznámá chyba</p>
[% END %]

[% IF whole.stage == "confirm" %]
<h2>Zrušit ručně zadaný požadavek</h2>
<p>Proceeding with this action will set this request to 'Cancelled'.</p>
<p>This means that actions have been taken to cancel this request at the source with whom the request was placed.</p>
<p>If you can confirm this has been done, please proceed.</p>
<p>
  [% base_url = "/cgi-bin/koha/ill/ill-requests.pl" %]
  [% proceed_url = base_url _ "?method=cancel&stage=confirm" _
                   "&illrequest_id=" _ request.illrequest_id %]
  <a class="btn btn-sm btn-default" href="[% proceed_url %]">Zrušit požadavek</a>
  <a class="btn btn-sm btn-default cancel" href="[% base_url %]">Zrušit</a>
</p>
[% END %]

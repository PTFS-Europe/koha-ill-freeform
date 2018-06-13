<h1>Place request with partner libraries</h1>
[% IF error %]
[% IF error == 'no_target_email' %]
	<div class="alert">
		No target email addresses found. Either select at least
		one partner or check your ILL partner library records.
	</div>
[% ELSIF error == 'no_library_email' %]
	<div class="alert">
		Your library has no usable email address. Please set it.
	</div>
[% ELSIF error == 'unkown_error' %]
	<div class="alert">
		Unknown error processing your request. Contact your administrator.
	</div>
[% END %]
[% END %]
<!-- Start of GENERIC_EMAIL case -->
[% IF whole.value.partners %]
	[% ill_url = "/cgi-bin/koha/ill/ill-requests.pl?method=illview&illrequest_id=" _ request.illrequest_id %]
	<form method="POST" action="/cgi-bin/koha/ill/ill-requests.pl">
		<fieldset class="rows">
			<legend>Interlibrary loan request details</legend>
			<ol>
				<li>
					<label for="partner_filter">Filter partner libraries:</label>
					<input type="text" id="partner_filter">
				</li>
				<li>
					<label for="partners" class="required">Select partner libraries:</label>
					<select size="5" multiple="true" id="partners" name="partners" required="required">
						[% FOREACH partner IN whole.value.partners %]
							<option value=[% partner.email %]>
								[% partner.branchcode _ " - " _ partner.surname %]
							</option>
						[% END %]
					</select>

				</li>
				<li>
					<label for="subject" class="required">Subject Line</label>
					<input type="text" name="subject" id="subject" type="text" value="[% whole.value.draft.subject %]" required="required" />
				</li>
				<li>
					<label for="body" class="required">Email text:</label>
					<textarea name="body" id="body" rows="20" cols="80" required="required">[% whole.value.draft.body %]</textarea>
				</li>
			</ol>
			<input type="hidden" value="generic_confirm" name="method">
			<input type="hidden" value="draft" name="stage">
			<input type="hidden" value="[% request.illrequest_id %]" name="illrequest_id">
		</fieldset>
		<fieldset class="action">
			<input type="submit" class="btn btn-default" value="Send email"/>
			<span><a href="[% ill_url %]" title="Return to request details">Cancel</a></span>
		</fieldset>
	</form>
[% ELSE %]
	<fieldset class="rows">
		<legend>Interlibrary loan request details</legend>
		<p>No partners have been defined yet. Please create appropriate patron records (by default ILLLIBS category).</p>
		<p>Be sure to provide email addresses for these patrons.</p>
		<p><span><a href="[% ill_url %]" title="Return to request details">Cancel</a></span></p>
	</fieldset>
[% END %]

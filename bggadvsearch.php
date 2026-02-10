<?php
// Search in multiple users BGG collections by Faguss
define("PAGE_TITLE","BGG Multi-User Advanced Search");
define("ENABLE_DOWNLOAD",1);
define("SLEEP_TIME_BETWEEN_USERS",1);
define("DOWNLOAD_URL_DOMAIN", "https://boardgamegeek.com");
define("DOWNLOAD_URL_PAGE", "/geeksearch.php?action=search&advsearch=1&objecttype=boardgame");
define("DOWNLOAD_URL_SUFFIX", "&B1=Submit");
define("USER_LIMIT", ["min"=>2, "max"=>10]);
define("PAGE_LIMIT", 4);
define("BACK_TO_FORM_VAR","showform");
define("MAP_INPUT_FIELDS_TO_QUERY_VARS",[
	"title" => "q",
	"include[designerid]" => "include[designerid]",
	"designer" => "geekitemname",
	"publisher" => "geekitemname",
	"include[publisherid]" => "include[publisherid]",
	"yearfrom" => "range[yearpublished][min]",
	"yearto" => "range[yearpublished][max]",
	"minimumage" => "range[minage][max]",
	"ratingfrom" => "floatrange[avgrating][min]",
	"ratingto" => "floatrange[avgrating][max]",
	"ratingnum" => "range[numvoters][min]",
	"weightfrom" => "floatrange[avgweight][min]",
	"weightto" => "floatrange[avgweight][max]",
	"weightnum" => "range[numweights][min]",
	"status" => "colfiltertype",
	"users" => "searchuser",
	"noexpansions" => "nosubtypes[]",
	"plrmin" => "range[minplayers][max]",
	"plrmax" => "range[maxplayers][min]",
	"plrrangetype" => "playerrangetype",
	"timemin" => "range[leastplaytime][min]",
	"timemax" => "range[playtime][max]",
	"category" => "propertyids[]",
	"category_exclude" => "nopropertyids[]",
	"domain" => "familyids[]",
	"domain_exclude" => "nofamilyids[]",
]);
define("INPUT_FIELDS_DEFAULT_VALUES", [
	"status" => "owned",
	"plrrangetype" => "normal",
]);
define("INPUT_FIELDS_TO_EXPLODE", [
	"domain",
	"domain_exclude",
	"category",
	"category_exclude"
]);
define("PLAYING_TIME", [
	"15" => "15 minutes",
	"30" => "30 minutes",
	"45" => "45 minutes",
	"60" => "60 minutes",
	"90" => "1.5 hours",
	"120" => "2 hours",
	"150" => "2.5 hours",
	"180" => "3 hours",
	"210" => "3.5 hours",
	"240" => "4 hours",
	"300" => "5 hours",
	"360" => "6+ hours",
]);
define("FORM_CHECKBOX_TABLES", ["Board Game Category","Board Game Mechanic","Board Game Subdomain"]);
define("BOARD_GAME_CATEGORY",[
	["name"=>"Abstract Strategy","id"=>1009],
	["name"=>"Action / Dexterity","id"=>1032],
	["name"=>"Adventure","id"=>1022],
	["name"=>"Age of Reason","id"=>2726],
	["name"=>"American Civil War","id"=>1048],
	["name"=>"American Indian Wars","id"=>1108],
	["name"=>"American Revolutionary War","id"=>1075],
	["name"=>"American West","id"=>1055],
	["name"=>"Ancient","id"=>1050],
	["name"=>"Animals","id"=>1089],
	["name"=>"Arabian","id"=>1052],
	["name"=>"Aviation / Flight","id"=>2650],
	["name"=>"Bluffing","id"=>1023],
	["name"=>"Book","id"=>1117],
	["name"=>"Card Game","id"=>1002],
	["name"=>"Children's Game","id"=>1041],
	["name"=>"City Building","id"=>1029],
	["name"=>"Civil War","id"=>1102],
	["name"=>"Civilization","id"=>1015],
	["name"=>"Collectible Components","id"=>1044],
	["name"=>"Comic Book / Strip","id"=>1116],
	["name"=>"Deduction","id"=>1039],
	["name"=>"Dice","id"=>1017],
	["name"=>"Economic","id"=>1021],
	["name"=>"Educational","id"=>1094],
	["name"=>"Electronic","id"=>1072],
	["name"=>"Environmental","id"=>1084],
	["name"=>"Expansion for Base-game","id"=>1042],
	["name"=>"Exploration","id"=>1020],
	["name"=>"Fan Expansion","id"=>2687],
	["name"=>"Fantasy","id"=>1010],
	["name"=>"Farming","id"=>1013],
	["name"=>"Fighting","id"=>1046],
	["name"=>"Game System","id"=>1119],
	["name"=>"Horror","id"=>1024],
	["name"=>"Humor","id"=>1079],
	["name"=>"Industry / Manufacturing","id"=>1088],
	["name"=>"Korean War","id"=>1091],
	["name"=>"Mafia","id"=>1033],
	["name"=>"Math","id"=>1104],
	["name"=>"Mature / Adult","id"=>1118],
	["name"=>"Maze","id"=>1059],
	["name"=>"Medical","id"=>2145],
	["name"=>"Medieval","id"=>1035],
	["name"=>"Memory","id"=>1045],
	["name"=>"Miniatures","id"=>1047],
	["name"=>"Modern Warfare","id"=>1069],
	["name"=>"Movies / TV / Radio theme","id"=>1064],
	["name"=>"Murder / Mystery","id"=>1040],
	["name"=>"Music","id"=>1054],
	["name"=>"Mythology","id"=>1082],
	["name"=>"Napoleonic","id"=>1051],
	["name"=>"Nautical","id"=>1008],
	["name"=>"Negotiation","id"=>1026],
	["name"=>"Novel-based","id"=>1093],
	["name"=>"Number","id"=>1098],
	["name"=>"Party Game","id"=>1030],
	["name"=>"Pike and Shot","id"=>2725],
	["name"=>"Pirates","id"=>1090],
	["name"=>"Political","id"=>1001],
	["name"=>"Post-Napoleonic","id"=>2710],
	["name"=>"Prehistoric","id"=>1036],
	["name"=>"Print & Play","id"=>1120],
	["name"=>"Puzzle","id"=>1028],
	["name"=>"Racing","id"=>1031],
	["name"=>"Real-time","id"=>1037],
	["name"=>"Religious","id"=>1115],
	["name"=>"Renaissance","id"=>1070],
	["name"=>"Science Fiction","id"=>1016],
	["name"=>"Space Exploration","id"=>1113],
	["name"=>"Spies / Secret Agents","id"=>1081],
	["name"=>"Sports","id"=>1038],
	["name"=>"Territory Building","id"=>1086],
	["name"=>"Third-party Expansion","id"=>3129],
	["name"=>"Trains","id"=>1034],
	["name"=>"Transportation","id"=>1011],
	["name"=>"Travel","id"=>1097],
	["name"=>"Trivia","id"=>1027],
	["name"=>"Video Game Theme","id"=>1101],
	["name"=>"Vietnam War","id"=>1109],
	["name"=>"Wargame","id"=>1019],
	["name"=>"Word Game","id"=>1025],
	["name"=>"World War I","id"=>1065],
	["name"=>"World War II","id"=>1049],
	["name"=>"Zombies","id"=>2481],
]);
define("BOARD_GAME_MECHANIC",[
	["name"=>"Acting","id"=>2073],
	["name"=>"Action / Event","id"=>2840],
	["name"=>"Action Drafting","id"=>2838],
	["name"=>"Action Points","id"=>2001],
	["name"=>"Action Queue","id"=>2689],
	["name"=>"Action Retrieval","id"=>2839],
	["name"=>"Action Timer","id"=>2834],
	["name"=>"Advantage Token","id"=>2847],
	["name"=>"Alliances","id"=>2916],
	["name"=>"Area Majority / Influence","id"=>2080],
	["name"=>"Area Movement","id"=>2046],
	["name"=>"Area-Impulse","id"=>2021],
	["name"=>"Auction / Bidding","id"=>2012],
	["name"=>"Auction Compensation","id"=>3098],
	["name"=>"Auction: Dexterity","id"=>2930],
	["name"=>"Auction: Dutch","id"=>2924],
	["name"=>"Auction: Dutch Priority","id"=>2932],
	["name"=>"Auction: English","id"=>2918],
	["name"=>"Auction: Fixed Placement","id"=>2931],
	["name"=>"Auction: Multiple Lot","id"=>2927],
	["name"=>"Auction: Once Around","id"=>2923],
	["name"=>"Auction: Sealed Bid","id"=>2920],
	["name"=>"Auction: Turn Order Until Pass","id"=>2919],
	["name"=>"Automatic Resource Growth","id"=>2903],
	["name"=>"Betting and Bluffing","id"=>2014],
	["name"=>"Bias","id"=>2957],
	["name"=>"Bids As Wagers","id"=>3097],
	["name"=>"Bingo","id"=>2999],
	["name"=>"Bribery","id"=>2913],
	["name"=>"Campaign / Battle Card Driven","id"=>2018],
	["name"=>"Card Play Conflict Resolution","id"=>2857],
	["name"=>"Catch the Leader","id"=>2887],
	["name"=>"Chaining","id"=>2956],
	["name"=>"Chit-Pull System","id"=>2057],
	["name"=>"Closed Drafting","id"=>2984],
	["name"=>"Closed Economy Auction","id"=>2928],
	["name"=>"Command Cards","id"=>2841],
	["name"=>"Commodity Speculation","id"=>2013],
	["name"=>"Communication Limits","id"=>2893],
	["name"=>"Connections","id"=>2883],
	["name"=>"Constrained Bidding","id"=>2922],
	["name"=>"Contracts","id"=>2912],
	["name"=>"Cooperative Game","id"=>2023],
	["name"=>"Crayon Rail System","id"=>2010],
	["name"=>"Critical Hits and Failures","id"=>2854],
	["name"=>"Cube Tower","id"=>2990],
	["name"=>"Deck Construction","id"=>3004],
	["name"=>"Deck, Bag, and Pool Building","id"=>2664],
	["name"=>"Deduction","id"=>3002],
	["name"=>"Delayed Purchase","id"=>2901],
	["name"=>"Dice Rolling","id"=>2072],
	["name"=>"Die Icon Resolution","id"=>2856],
	["name"=>"Different Dice Movement","id"=>2950],
	["name"=>"Drawing","id"=>3096],
	["name"=>"Elapsed Real Time Ending","id"=>2882],
	["name"=>"Enclosure","id"=>2043],
	["name"=>"End Game Bonuses","id"=>2875],
	["name"=>"Events","id"=>2850],
	["name"=>"Finale Ending","id"=>2885],
	["name"=>"Flicking","id"=>2860],
	["name"=>"Follow","id"=>2843],
	["name"=>"Force Commitment","id"=>2864],
	["name"=>"Grid Coverage","id"=>2978],
	["name"=>"Grid Movement","id"=>2676],
	["name"=>"Hand Management","id"=>2040],
	["name"=>"Hexagon Grid","id"=>2026],
	["name"=>"Hidden Movement","id"=>2967],
	["name"=>"Hidden Roles","id"=>2891],
	["name"=>"Hidden Victory Points","id"=>2987],
	["name"=>"Highest-Lowest Scoring","id"=>2889],
	["name"=>"Hot Potato","id"=>3000],
	["name"=>"I Cut, You Choose","id"=>2906],
	["name"=>"Impulse Movement","id"=>2952],
	["name"=>"Income","id"=>2902],
	["name"=>"Increase Value of Unchosen Resources","id"=>2914],
	["name"=>"Induction","id"=>3003],
	["name"=>"Interrupts","id"=>2837],
	["name"=>"Investment","id"=>2910],
	["name"=>"Kill Steal","id"=>2871],
	["name"=>"King of the Hill","id"=>2886],
	["name"=>"Ladder Climbing","id"=>2980],
	["name"=>"Layering","id"=>3001],
	["name"=>"Legacy Game","id"=>2824],
	["name"=>"Line Drawing","id"=>2039],
	["name"=>"Line of Sight","id"=>2975],
	["name"=>"Loans","id"=>2904],
	["name"=>"Lose a Turn","id"=>2836],
	["name"=>"Mancala","id"=>2955],
	["name"=>"Map Addition","id"=>2959],
	["name"=>"Map Deformation","id"=>2961],
	["name"=>"Map Reduction","id"=>2960],
	["name"=>"Market","id"=>2900],
	["name"=>"Matching","id"=>3007],
	["name"=>"Measurement Movement","id"=>2949],
	["name"=>"Melding and Splaying","id"=>2981],
	["name"=>"Memory","id"=>2047],
	["name"=>"Minimap Resolution","id"=>2863],
	["name"=>"Modular Board","id"=>2011],
	["name"=>"Move Through Deck","id"=>2962],
	["name"=>"Movement Points","id"=>2947],
	["name"=>"Movement Template","id"=>2963],
	["name"=>"Moving Multiple Units","id"=>2958],
	["name"=>"Multi-Use Cards","id"=>3099],
	["name"=>"Multiple Maps","id"=>2965],
	["name"=>"Narrative Choice / Paragraph","id"=>2851],
	["name"=>"Negotiation","id"=>2915],
	["name"=>"Neighbor Scope","id"=>3104],
	["name"=>"Network and Route Building","id"=>2081],
	["name"=>"Once-Per-Game Abilities","id"=>2846],
	["name"=>"Open Drafting","id"=>2041],
	["name"=>"Order Counters","id"=>2844],
	["name"=>"Ordering","id"=>3101],
	["name"=>"Ownership","id"=>2911],
	["name"=>"Paper-and-Pencil","id"=>2055],
	["name"=>"Passed Action Token","id"=>2835],
	["name"=>"Pattern Building","id"=>2048],
	["name"=>"Pattern Movement","id"=>2946],
	["name"=>"Pattern Recognition","id"=>2060],
	["name"=>"Physical Removal","id"=>2989],
	["name"=>"Pick-up and Deliver","id"=>2007],
	["name"=>"Pieces as Map","id"=>2964],
	["name"=>"Player Elimination","id"=>2685],
	["name"=>"Player Judge","id"=>2865],
	["name"=>"Point to Point Movement","id"=>2078],
	["name"=>"Predictive Bid","id"=>3006],
	["name"=>"Prisoner's Dilemma","id"=>2858],
	["name"=>"Programmed Movement","id"=>2953],
	["name"=>"Push Your Luck","id"=>2661],
	["name"=>"Questions and Answers","id"=>3102],
	["name"=>"Race","id"=>2876],
	["name"=>"Random Production","id"=>2909],
	["name"=>"Ratio / Combat Results Table","id"=>2855],
	["name"=>"Re-rolling and Locking","id"=>2870],
	["name"=>"Real-Time","id"=>2831],
	["name"=>"Relative Movement","id"=>2954],
	["name"=>"Resource Queue","id"=>3103],
	["name"=>"Resource to Move","id"=>2948],
	["name"=>"Rock-Paper-Scissors","id"=>2003],
	["name"=>"Role Playing","id"=>2028],
	["name"=>"Roles with Asymmetric Information","id"=>2892],
	["name"=>"Roll / Spin and Move","id"=>2035],
	["name"=>"Rondel","id"=>2813],
	["name"=>"Scenario / Mission / Campaign Game","id"=>2822],
	["name"=>"Score-and-Reset Game","id"=>2823],
	["name"=>"Secret Unit Deployment","id"=>2016],
	["name"=>"Selection Order Bid","id"=>2926],
	["name"=>"Semi-Cooperative Game","id"=>2820],
	["name"=>"Set Collection","id"=>2004],
	["name"=>"Simulation","id"=>2070],
	["name"=>"Simultaneous Action Selection","id"=>2020],
	["name"=>"Singing","id"=>2038],
	["name"=>"Single Loser Game","id"=>2821],
	["name"=>"Slide / Push","id"=>3005],
	["name"=>"Solo / Solitaire Game","id"=>2819],
	["name"=>"Speed Matching","id"=>2991],
	["name"=>"Spelling","id"=>3113],
	["name"=>"Square Grid","id"=>2940],
	["name"=>"Stacking and Balancing","id"=>2988],
	["name"=>"Stat Check Resolution","id"=>2853],
	["name"=>"Static Capture","id"=>2861],
	["name"=>"Stock Holding","id"=>2005],
	["name"=>"Storytelling","id"=>2027],
	["name"=>"Sudden Death Ending","id"=>2884],
	["name"=>"Tags","id"=>3100],
	["name"=>"Take That","id"=>2686],
	["name"=>"Targeted Clues","id"=>2866],
	["name"=>"Team-Based Game","id"=>2019],
	["name"=>"Tech Trees / Tech Tracks","id"=>2849],
	["name"=>"Three Dimensional Movement","id"=>2944],
	["name"=>"Tile Placement","id"=>2002],
	["name"=>"Track Movement","id"=>2939],
	["name"=>"Trading","id"=>2008],
	["name"=>"Traitor Game","id"=>2814],
	["name"=>"Trick-taking","id"=>2009],
	["name"=>"Tug of War","id"=>2888],
	["name"=>"Turn Order: Auction","id"=>2827],
	["name"=>"Turn Order: Claim Action","id"=>2829],
	["name"=>"Turn Order: Pass Order","id"=>2830],
	["name"=>"Turn Order: Progressive","id"=>2828],
	["name"=>"Turn Order: Random","id"=>2985],
	["name"=>"Turn Order: Role Order","id"=>2833],
	["name"=>"Turn Order: Stat-Based","id"=>2826],
	["name"=>"Turn Order: Time Track","id"=>2663],
	["name"=>"Variable Phase Order","id"=>2079],
	["name"=>"Variable Player Powers","id"=>2015],
	["name"=>"Variable Set-up","id"=>2897],
	["name"=>"Victory Points as a Resource","id"=>2874],
	["name"=>"Voting","id"=>2017],
	["name"=>"Worker Placement","id"=>2082],
	["name"=>"Worker Placement with Dice Workers","id"=>2935],
	["name"=>"Worker Placement, Different Worker Types","id"=>2933],
	["name"=>"Zone of Control","id"=>2974],
]);
define("BOARD_GAME_SUBDOMAIN",[
	["name"=>"Abstract Games","id"=>4666],
	["name"=>"Children's Games","id"=>4665],
	["name"=>"Customizable Games","id"=>4667],
	["name"=>"Family Games","id"=>5499],
	["name"=>"Party Games","id"=>5498],
	["name"=>"Strategy Games","id"=>5497],
	["name"=>"Thematic Games","id"=>5496],
	["name"=>"Wargames","id"=>4664],
]);
define("SEARCH_SUMMARY",[
	"Title",
	"Designer",
	"Publisher",
	"Year Released Range",
	"Minimum Age",
	"Average User Rating Range",
	"Average Gameplay Weight Range",
	"Users",
	"Status",
	"Exclude expansions",
	"Players Range",
	"Playing Time",
	"Board Game Category",
	"Board Game Mechanic",
	"Board Game Subdomain",
]);
define("SEARCH_RESULT_TABLE_COLUMNS", [
	0 => "Rank",
	1 => "Thumbnail",
	2 => "Title",
	3 => "Geek Rating",
	4 => "Avg Rating",
	5 => "Num Voters",
	6 => "Shop"
]);

function print_select($name, $array, $js_func="") {
	?>
	<select id="<?=$name?>" name="<?=$name?>" onchange="<?=$js_func?>">
	<option></option>
	<?php
	foreach ($array as $value=>$description) 
		echo "<option value=\"$value\"".(get_input_var($name)==$value?' selected="selected"':'').">$description</option>";
	?>
	</select>
	<?php
}
	
function print_checkbox_table($name, $checkbox_name) {
	$id_name = str_replace(" ","_",strtolower($name));
	$list_include = isset($_GET[$checkbox_name]) ? explode(",",$_GET[$checkbox_name]) : [];
	$list_exclude = isset($_GET[$checkbox_name."_exclude"]) ? explode(",",$_GET[$checkbox_name."_exclude"]) : [];
	$check_count = 0;
	
	?>
	<div class="input_container">
		<b class="fake_link" onclick="toggle_display('<?=$id_name?>')"><?=$name?> &gt;&gt;</b>
		<table id="<?=$id_name?>" class="checkbox_table">
		<tbody>
		<?php
		$i = 0;
		$max_columns = 3;
		foreach(constant(strtoupper($id_name)) as $item) {
			$check_include = in_array($item["id"],$list_include);
			$check_exclude = in_array($item["id"],$list_exclude);
			
			if ($check_include || $check_exclude)
				$check_count++;
			
			if ($i == 0) 
				echo "<tr>";
			
			?>
			<td>
			<label for="checkbox_i<?=$item["id"]?>"><?=$item["name"]?></label>
			+<input id="checkbox_i<?=$item["id"]?>" type="checkbox" name="<?=$checkbox_name?>[]" value="<?=$item["id"]?>" <?=$check_include?"checked":""?>>
			-<input id="checkbox_e<?=$item["id"]?>" type="checkbox" name="<?=$checkbox_name?>_exclude[]" value="<?=$item["id"]?>" <?=$check_exclude?"checked":""?>>
			</td>
			<?php
			
			if (++$i == $max_columns) {
				echo "</tr>";
				$i = 0;
			}
		}
		?>
		</tbody>
		</table>
	</div>
	<?php
	return $check_count;
}

function url_get_contents($url, $timeout=NULL) {
    if (!function_exists('curl_init'))
        die('CURL is not installed!');

    $request = curl_init();
	curl_setopt($request, CURLOPT_URL, $url);
	curl_setopt($request, CURLOPT_HEADER, 0);
	curl_setopt($request, CURLOPT_RETURNTRANSFER, 1);
	
	if (isset($timeout))
		curl_setopt($request, CURLOPT_TIMEOUT, $timeout);
	
    $output = curl_exec($request);
    curl_close($request);
    return $output;
}

function compare_rank($a, $b) {
	$a_rank = $a["Rank"];
	$b_rank = $b["Rank"];
	
	if ($a_rank == $b_rank)
		return 0;
	else
		return $a_rank > $b_rank ? 1 : -1;
}

function DOMinnerHTML(DOMNode $element) { 
	$innerHTML = ""; 
	foreach($element->childNodes as $child)
		$innerHTML .= $element->ownerDocument->saveHTML($child);
	return $innerHTML; 
} 

function process_downloaded_bgg_search($input_html, $input_username, &$output, &$user_details) {
	$game_count = 0;
	
	if (empty($input_html))
		return $game_count;
	
	$doc = new DOMDocument();
	libxml_use_internal_errors(true);	//hide warnings
	$doc->loadHtml($input_html);

	$xpath = new DOMXPath($doc);
	$div = $doc->getElementById('collectionitems');

	if (isset($div))
		$user_details[$input_username]["status"] = 1;
	else
		return $game_count;

	$query = '//table/tr';
	$rows = $xpath->query($query, $div);
	
	foreach($rows as $i => $row) {
		$id = $row->attributes->getNamedItem('id');
		if (!$id)
			continue;
		
		if (substr($id->value, 0, 4) != "row_")
			continue;

		$column_index = 0;
		$current_game = [];
		$current_game_title = "";
		
		foreach($row->childNodes as $column) {
			if ($column->nodeName == "td") {
				$value_to_copy = DOMinnerHTML($column);
				
				if ($column_index == array_search("Title", SEARCH_RESULT_TABLE_COLUMNS)) {
					$div = $column->childNodes[3];
					$a = $div->childNodes[1];
					$a_html = $div->ownerDocument->saveHTML($a);
					
					$current_game_title = $a->nodeValue;
					$year = isset($div->childNodes[3]->nodeValue) ? $div->childNodes[3]->nodeValue : "";
					$description = isset($column->childNodes[5]) ? trim($column->childNodes[5]->nodeValue) : "";
					
					$url = "";
					$to_find = 'href="';
					$pos = strpos($a_html, $to_find);
					if ($pos !== FALSE) {
						$pos += strlen($to_find);
						$pos2 = strpos($a_html, '"', $pos);
						if ($pos2 !== FALSE)
							$url = substr($a_html, $pos, $pos2-$pos);
					}
					
					$value_to_copy = "<div><a class='game_title' href='".DOWNLOAD_URL_DOMAIN."$url' target='_blank'>$current_game_title</a> <span class='game_info'>$year</span></div><p class='game_info' style='margin: 2px 0 0 0;'>$description</p>";					
				}
				
				if ($column_index == array_search("Thumbnail", SEARCH_RESULT_TABLE_COLUMNS)) {
					$value_to_copy = str_replace("href=\"","target=\"_blank\" href=\"".DOWNLOAD_URL_DOMAIN,$value_to_copy);
				}
				
				if ($column_index == array_search("Rank", SEARCH_RESULT_TABLE_COLUMNS)) {
					$nv = trim($column->nodeValue);
					$value_to_copy = is_numeric($nv) ? intval($nv) : PHP_INT_MAX;
				}
				
				if ($column_index == array_search("Avg Rating", SEARCH_RESULT_TABLE_COLUMNS)) {
					$nv = trim($column->nodeValue);
					$value_to_copy = is_numeric($nv) ? floatval($nv) : PHP_FLOAT_MAX;
				}
				
				$current_game[SEARCH_RESULT_TABLE_COLUMNS[$column_index]] = $value_to_copy;
				$column_index++;
			}
		}
		
		if (array_key_exists($current_game_title,$output)) {
			$output[$current_game_title]["Status"][] = $input_username;
		} else {
			$output[$current_game_title] = [
				"Rank"        => $current_game["Rank"],
				"Thumbnail"   => $current_game["Thumbnail"],
				"Title"       => $current_game["Title"],
				"Avg Rating"  => $current_game["Avg Rating"],
				"Status"      => [$input_username],
			];
		}
		
		$game_count++;
		$user_details[$input_username]["game_count"]++;
	}
	
	return $game_count;
}

function get_input_var($name, $default="") {
	return isset($_GET[$name]) ? $_GET[$name] : $default;
}

$how_many_tables_checked = [];
?>
<!DOCTYPE html>
<html lang="en">
<head>
	<link rel="preconnect" href="https://fonts.googleapis.com">
	<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
	<link href="https://fonts.googleapis.com/css2?family=Figtree:ital,wght@0,300..900;1,300..900&family=Noto+Sans:ital,wght@0,100..900;1,100..900&display=swap" rel="stylesheet">
	<link rel="icon" href="data:,">
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<meta http-equiv="X-UA-Compatible" content="ie=edge">
	<style>
	body {
		background-color:#e7e7ea;
	}
	a:link, a:visited {
		color: #005bca;
		text-decoration: none;
		font-weight: bold;
	}
	a:hover {
		text-decoration: underline;
	}
	html * {
		font-family: "Figtree", sans-serif;
	}
	form {
		margin-top: 40px;
	}
	.input_container {
		margin-top: 20px;
		margin-bottom: 20px;
	}
	.checkbox_table td {
		font-size: small;
	}
	.fake_link {
		color: #005bca;
		cursor: pointer;
	}
	button {
		height: 30px;
		width: 100px;
		margin-right: 60px;
	}
	#playersrangedescription {
		font-size: small;
	}
	
	.general_container {
		background-color:#ffffff;
		max-width: 1340px;
		margin-bottom: 10px;
		padding: 11px;
		border-radius: 10px;
		margin-left: auto;
		margin-right: auto;
	}
	li {
		font-size: small;
		margin-bottom: 2px;
	}
	.small_link {
		font-size: x-small;
	}
	.search_results td {
		border-left: 1px solid #d1d4d7;
		border-top: 1px solid #d1d4d7;
		font-size: 13px;
		padding-left: 2px;
		padding-right: 2px;
		padding-bottom: 5px;
	}
	.search_results td:last-child {
		border-right: 1px solid #d1d4d7;
	}
	.search_results tr:last-child td {
		border-bottom: 1px solid #d1d4d7;
	}
	.rank {
		font-weight: 700;
		max-width: 40px;
	}
	.image {
		text-align: center;
		width: 100px;
	}
	img {
		margin-top: 10px;
	}
	.game_info {
		color: #585f65;
	}
	.search_results .title {
		padding-left: 10px;
		padding-right: 10px;
	}
	.game_title {
		font-size: 14px;
		font-weight: 700;
	}
	</style>
	<title><?=PAGE_TITLE?></title>
</head>
<body>

<?php
if (!empty($_GET) && empty($_GET[BACK_TO_FORM_VAR])) {
	$url_base = DOWNLOAD_URL_DOMAIN.DOWNLOAD_URL_PAGE;
	$all_games = [];
	$usernames = [];
	$user_details = [];
	
	if (ENABLE_DOWNLOAD) {
		foreach(preg_split("/,/", get_input_var("users")) as $name) {	// bgg usernames may contain spaces so split by comma
			$name = trim($name);
			if (strlen($name)>=4 && strlen($name)<=20)	// bgg username character range
				$usernames[] = $name;
		}
		
		if (count($usernames) >= USER_LIMIT["min"]) {
			$usernames = array_slice($usernames, 0, USER_LIMIT["max"]);
			
			foreach(MAP_INPUT_FIELDS_TO_QUERY_VARS as $input_field_name=>$query_var_name) {
				$query_var_name = str_replace(["[","]"],["%5B","%5D"],$query_var_name); // bgg requires url encoded in this way
				$value = get_input_var($input_field_name);
				
				if ($input_field_name == "users")
					$value = "???";
				
				if ($input_field_name == "noexpansions") {
					if (empty($value))
						continue;
					else
						$value = "boardgameexpansion";
				}
				
				if (in_array($input_field_name,array_keys(INPUT_FIELDS_DEFAULT_VALUES)) && $value=="")
					$value = INPUT_FIELDS_DEFAULT_VALUES[$input_field_name];
				
				if (in_array($input_field_name,INPUT_FIELDS_TO_EXPLODE)) {
					if (!empty($value))
						foreach(explode(",",$value) as $item)
							$url_base .= "&$query_var_name=$item";
							
					continue;
				}
				
				$url_base .= "&$query_var_name=$value";
			}
			
			foreach($usernames as $index=>$name) {
				$url_for_current_user = str_replace("???",$name,$url_base).DOWNLOAD_URL_SUFFIX;
				$user_details[$name] = ["url"=>$url_for_current_user, "game_count"=>0, "status"=>0];
				$page_count = 0;
				
				if ($index != 0) 
					sleep(SLEEP_TIME_BETWEEN_USERS);
				
				do {
					$page_content = url_get_contents($url_for_current_user);
					process_downloaded_bgg_search($page_content, $name, $all_games, $user_details);
					
					$url_for_current_user = "";
					$next_page = strpos($page_content, "title=\"next page\"");
					if ($next_page !== FALSE) {
						$to_find = "href=\"";
						$start_pos = strrpos($page_content, $to_find, -(strlen($page_content) - $next_page));
						if ($start_pos !== FALSE) {
							$start_pos += strlen($to_find);
							$end_pos = strpos($page_content, "\"", $start_pos);
							if ($end_pos !== FALSE) {
								$url_for_current_user = DOWNLOAD_URL_DOMAIN . html_entity_decode(substr($page_content, $start_pos, $end_pos-$start_pos));
								sleep(SLEEP_TIME_BETWEEN_USERS/10);
							}
						}
					}
				} while (!empty($url_for_current_user) && ++$page_count < PAGE_LIMIT);
			}
		} else {
			echo "Must give at least ".USER_LIMIT["min"]." usernames";
		}
	} else {
		$files_to_process = ["test4","test3","test2","test1"];
		
		foreach($files_to_process as $file) {
			$user_details[$file] = ["url"=>$file.'.htm', "game_count"=>0, "status"=>0];
			echo process_downloaded_bgg_search(file_get_contents($file.'.htm'), $file, $all_games, $user_details)."<br>";
		}
	}

	usort($all_games, 'compare_rank');
	
	$url_back = (empty($_SERVER["HTTPS"]) ? "http" : "https") . "://" . $_SERVER["HTTP_HOST"] . $_SERVER["REQUEST_URI"];
	$var_to_add = "&".BACK_TO_FORM_VAR."=1";
	if (strpos($url_back,$var_to_add) === FALSE)
		$url_back .= $var_to_add;
	$back_to_form_link = '<a href="'.$url_back.'">Back to search form</a>';
	?>
	
	<div class="general_container">
	<p style="margin:0"><b><?=PAGE_TITLE?></b> - <?=count($all_games)?> result<?=count($all_games)!=1?"s":""?></p>
	<ul style="margin-bottom:8px;margin-top:8px;">
	<?php
	foreach(SEARCH_SUMMARY as $title) {
		$value = "";
		
		switch($title) {
			case "Title" : 
			case "Designer" :
			case "Publisher" : 
				$value = get_input_var(strtolower($title)); 
				break;
				
			case "Minimum Age" :
				$value = get_input_var(str_replace(" ","",strtolower($title)))."+ y.o.";
				break;
			
			case "Year Released Range" : 
				$min = get_input_var("yearfrom");
				$max = get_input_var("yearto");
				
				if ($min!="" && $max!="")
					$value = "$min - $max";
				else
					if ($min!="" && $max=="")
						$value = "since $min";
					else
						if ($min=="" && $max!="")
							$value = "up to $max";
				break;
			
			case "Average User Rating Range" : 
			case "Average Gameplay Weight Range" :
				$prefix = $title == "Average User Rating Range" ? "rating" : "weight";
				$l = get_input_var($prefix."from","...");
				$r = get_input_var($prefix."to","...");
				if ($l!="..." || $r!="...")
					$value = $l." - ".$r;				
				if (get_input_var($prefix."num")!="")
					$value .= " with at least " . get_input_var($prefix."num") . " ratings";
				break;
				
			case "Users" :
				$bgg_var = get_input_var("status")!="" ? "rated" : "own";
				$adjective = get_input_var("status")!="" ? "Rated" : "Owned";
				$sorted_names = array_keys($user_details);
				sort($sorted_names);
				echo "<li><b>$adjective by users:</b><ul>";
				
				foreach($sorted_names as $username) {
					$url_collection = "<a target='_blank' href='".DOWNLOAD_URL_DOMAIN."/collection/user/{$username}?{$bgg_var}=1'><span class='small_link'>link to ".strtolower($adjective)."</span></a>";
					$result = "failed";
					
					if ($user_details[$username]["status"])
						$result = $user_details[$username]["game_count"]." result".($user_details[$username]["game_count"]!=1?"s":"");
					
					echo "<li>$username ($result) &nbsp; $url_collection &nbsp; <a target='_blank' href='{$user_details[$username]["url"]}'><span class='small_link'>link to search</span></a></li>";
				}
				echo "</ul></li>";
				break;
				
			case "Exclude expansions" : 
				$value = get_input_var("noexpansions") != "" ? "yes" : ""; 
				break;
			
			case "Players Range" : 
				$min = get_input_var("plrmin");
				$max = get_input_var("plrmax");
				if (get_input_var("plrrangetype") == "") {
					$value = "";

					if ($min != "")
						$value .= "can be started with ".($min>1 ? ($min." or less players") : "1 player");

					if ($max != "")
						$value .= ($value!=""?"; ":"") . "playable with $max or more player" . ($max!=1?"s":"");
				} else
					$value = "$min - $max";
				break;
				
			case "Playing Time" : 
				$min = (isset(PLAYING_TIME[get_input_var("timemin")])) ? PLAYING_TIME[get_input_var("timemin")] : "";
				$max = (isset(PLAYING_TIME[get_input_var("timemax")])) ? PLAYING_TIME[get_input_var("timemax")] : "";
				if ($min!="" && $max!="")
					$value = "$min - $max";
				else
					if ($min!="" && $max=="")
						$value = "at least $min";
					else
						if ($min=="" && $max!="")
							$value = "up to $max";
				break;
			
			case "Board Game Category" :
			case "Board Game Mechanic" :
			case "Board Game Subdomain" : {
				$list_name = str_replace(" ","_",strtoupper($title));
				$input_var_name = $title=="Board Game Subdomain" ? "domain" : "category";
				$vars_to_process = [$input_var_name, $input_var_name."_exclude"];
				$results = [[],[]];
				$item_count = 0;
				
				foreach($vars_to_process as $var_index=>$var_name) {
					$var_value = isset($_GET[$var_name]) ? explode(",",$_GET[$var_name]) : [];

					foreach($var_value as $item) {
						$list = constant($list_name);
						for($i=0; $i<count($list); $i++) {
							if (intval($item) == $list[$i]["id"]) {
								$results[$var_index][] = $list[$i]["name"];
								$item_count++;
								break;
							}
						}
					}
				}
				
				if ($item_count > 0) {
					echo "<li><b>$title</b> ";
					foreach($results as $index=>$result) {
						if (!empty($result)) {
							echo "<ul><li>".($index==0 ? "include:" : "exclude:") . "<ul>";
							foreach($result as $item)
								echo "<li>$item</li>";
							echo "</ul></ul>";
						}
					}
					echo "</li>";
				}
			}
			break;
			
		}
		
		if (!empty($value))
			echo "<li><b>$title:</b> $value</li>";
	}
	?>
	</ul>
	<span style="font-size:small"><?=$back_to_form_link?></span>
	</div>
	
	<div class="general_container">
		<table class="search_results" cellpadding="0" cellspacing="1" class="collection_table" width="100%" id="collectionitems">
			<thead>
				<tr>
					<th class="rank">Rank <span style="font-family:sans-serif;color:#0000ff">&#9207;</span></th>
					<th class="image"></th>
					<th class="title">Title</th>
					<th class="title">Users</th>
				</tr>
			</thead>
			<tbody>
				<?php
				foreach($all_games as $game) {
					$rank = $game["Rank"]==PHP_INT_MAX ? "N/A" : $game["Rank"];
					$avgrating = $game["Avg Rating"]==PHP_FLOAT_MAX ? "N/A" : $game["Avg Rating"];
					$status = '';
					
					foreach($game["Status"] as $user) {
						if (!empty($status))
							$status .= ', ';
						
						$status .= '<a class="dummy_link" target="_blank" href="'.$user_details[$user]["url"].'">'.$user.'</a>';
					};
					?>
					<tr>
					<td class="rank" align="center"><?=$rank?></td>
					<td class="image"><?=$game["Thumbnail"]?></td>
					<td class="title"><?=$game["Title"]?></td>
					<td class="title"><?=$status?></td>
					</tr>
					<?php
				}
				?>
			</tbody>
		</table>
	</div>
	
	<?=$back_to_form_link?>
	<?php
} else {
	?>
	<h2><?=PAGE_TITLE?></h2>
	<p>Request BGG search for each given user and then merge results into a single table.</p>
	<form id="main_form">
		<div class="input_container">
			<label for="title" class="input"><b>Title:</b></label>
			<input id="title" type="text" name="title" value="<?=get_input_var("title")?>">
		</div>
		
		<!-- requires geekid
		<div class="input_container">
			<label for="designer" class="input"><b>Designer:</b></label>
			<input id="designer" type="text" name="designer" value="<?=get_input_var("designer")?>">
		</div>
		
		<div class="input_container">
			<label for="publisher" class="input"><b>Publisher:</b></label>
			<input id="publisher" type="text" name="publisher" value="<?=get_input_var("publisher")?>">
		</div>
		-->
		
		<div class="input_container">
			<label for="yearfrom" class="input"><b>Year Released Range:</b></label>
			<input id="yearfrom" type="text" name="yearfrom" value="<?=get_input_var("yearfrom")?>" size="3">
			to
			<input id="yearto" type="text" name="yearto" value="<?=get_input_var("yearto")?>" size="3">
		</div>
		
		<div class="input_container">
			<label for="minimumage" class="input"><b>Minimum Age:</b></label>
			<input id="minimumage" type="text" name="minimumage" value="<?=get_input_var("minimumage")?>" size="2">
		</div>
		
		<div class="input_container">
			<label for="ratingfrom" class="input"><b>Average User Rating Range:</b></label>
			<input id="ratingfrom" type="text" name="ratingfrom" value="<?=get_input_var("ratingfrom")?>" size="1">
			to
			<input id="ratingto" type="text" name="ratingto" value="<?=get_input_var("ratingto")?>" size="1">
			with at least
			<input id="ratingnum" type="text" name="ratingnum" value="<?=get_input_var("ratingnum")?>" size="2">
			user ratings
		</div>
		
		<div class="input_container">
			<label for="weightfrom" class="input"><b>Average Gameplay Weight Range:</b></label>
			<input id="weightfrom" type="text" name="weightfrom" value="<?=get_input_var("weightfrom")?>" size="1">
			to
			<input id="weightto" type="text" name="weightto" value="<?=get_input_var("weightto")?>" size="1">
			with at least
			<input id="weightnum" type="text" name="weightnum" value="<?=get_input_var("weightnum")?>" size="2">
			user weight ratings
		</div>
		
		<div class="input_container">
			<label for="users" class="input"><b>Specify Users:</b></label>
			<input id="users" type="text" name="users" value="<?=get_input_var("users")?>" size="50" placeholder="Adam,Jane">
			<br>
			<ul>
				<li>Max <?=USER_LIMIT["max"]?> usernames</li>
				<li>Max <?=PAGE_LIMIT?> pages (<?=PAGE_LIMIT*100?> results) per one user</li>
				<li><?=SLEEP_TIME_BETWEEN_USERS?> second delay between users to avoid requesting BGG too frequently</li>
			</ul>			
			<input type="radio" id="statusowned" name="status" value="owned" <?=get_input_var("status")==""?"checked":""?>>
			<label for="statusowned">Owned</label><br>
			
			<input type="radio" id="statusrated" name="status" value="rated" <?=get_input_var("status")!=""?"checked":""?>>
			<label for="statusrated">Rated</label> 
		</div>
		
		<div class="input_container">	
			<input type="checkbox" id="noexpansions" name="noexpansions" value="1" <?=get_input_var("noexpansions")=="1"?"checked":""?>>
			<label for="noexpansions">Exclude expansions</label>
		</div>
		
		<div class="input_container">
			<label for="plrmin"><b>Players Range:</b></label>
			<?=print_select("plrmin", array_diff(range(0,10),[0]), "describe_player_range('plrmin','plrmax','playersrangedescription')")?> 
			to: 
			<?=print_select("plrmax", array_diff(range(0,10),[0]), "describe_player_range('plrmin','plrmax','playersrangedescription')")?>
			<br>
			<input type="radio" id="plrrangetypenormal" name="plrrangetype" value="normal" <?=get_input_var("plrrangetype")==""?"checked":""?>>
			<label for="plrrangetypenormal">Approximate <span id="playersrangedescription"></span></label>
			<br>
			<input type="radio" id="plrrangetypeexclusive" name="plrrangetype" value="exclusive" <?=get_input_var("plrrangetype")!=""?"checked":""?>>
			<label for="plrrangetypeexclusive">Exact match</label><br>	
		</div>
		
		<div class="input_container">
			<label for="timemin"><b>Playing Time:</b></label>
			<?=print_select("timemin",PLAYING_TIME)?>
			to:
			<?=print_select("timemax",PLAYING_TIME)?>
		</div>
		
		<?php
		foreach(FORM_CHECKBOX_TABLES as $item)
			$how_many_tables_checked[] = print_checkbox_table($item, $item=="Board Game Subdomain" ? "domain" : "category");
		?>
		
		<div class="input_container">
			<button>Submit</button> <button type="button" onClick="reset_form()">Reset</button>
		</div>
	</form>
	<?php
}
?>

<script>
const INPUT_FIELDS_DEFAULT_VALUES = <?=json_encode(INPUT_FIELDS_DEFAULT_VALUES)?>;
const FORM_CHECKBOX_TABLES = <?=json_encode(FORM_CHECKBOX_TABLES)?>;
const FORM_CHECKBOX_TABLES_CHECKED = <?=json_encode($how_many_tables_checked)?>;

addEventListener('submit', (event) => {
	event.preventDefault();
	let new_query = "";
	let arg_count = 0;
	let usernames = [];
	let categories = {
		domain:[],
		domain_exclude:[],
		category:[],
		category_exclude:[]
	};
	
	// Remove empty query arguments for a nicer url
	for (let i=0; i<event.target.length; i++) {
		if (event.target[i].value) {
			let new_value = event.target[i].value;
			
			if (event.target[i].type=='checkbox' || event.target[i].type=='radio')
				new_value = event.target[i].checked ? new_value : '';
			
			if (
				(INPUT_FIELDS_DEFAULT_VALUES[event.target[i].name] && INPUT_FIELDS_DEFAULT_VALUES[event.target[i].name] == new_value) ||
				event.target[i].name == "<?=BACK_TO_FORM_VAR?>"
			)
				new_value = '';
			
			if (event.target[i].name == 'users')
				usernames = new_value.split(/,/);
			
			cat_name = event.target[i].name.replace('[]','');
			if (Object.keys(categories).includes(cat_name) && new_value!='') {
				categories[cat_name].push(new_value);
				new_value = '';
			}
			
			if (new_value != '')
				new_query = new_query + (arg_count++ ? '&' : '?') + event.target[i].name + '=' + new_value;
		}
	}

	// Instead of having a var for every checkbox make one var with values separated by commas for a nicer url
	for (const [key, value] of Object.entries(categories))
		if (value.length > 0)
			new_query = new_query + (arg_count++ ? '&' : '?') + key + '=' + value.join(',');
	
	if (new_query == '')
		alert('Nothing to search')
	else 
		if (usernames.length < <?=USER_LIMIT['min']?>)
			alert('Must give at least < <?=USER_LIMIT['min']?> usernames');
		else
			window.location.href = new_query;
});

document.addEventListener('DOMContentLoaded', function(event) {
	for(let i=0; i<FORM_CHECKBOX_TABLES.length; i++) {
		if (FORM_CHECKBOX_TABLES_CHECKED[i] == 0) {
			item = document.getElementById(FORM_CHECKBOX_TABLES[i].replaceAll(' ','_').toLowerCase());
			if (item)
				item.style.display = 'none';
		}
	}
	
	describe_player_range('plrmin','plrmax','playersrangedescription');
});

function toggle_display(item_name) {
	item = document.getElementById(item_name);
	if (item)
		item.style.display = (item.style.display == 'none') ? 'block' : 'none';
}

function reset_form() {
	let inputs = document.getElementsByTagName("input");
	for (let i=0; i<inputs.length; i++) {
		switch(inputs[i].type) {
			case 'checkbox' : inputs[i].checked=false; break;
			case 'text' : if (inputs[i].name!='users') inputs[i].value=''; break;
			case 'radio' :
				if (INPUT_FIELDS_DEFAULT_VALUES[inputs[i].name])
					inputs[i].checked = INPUT_FIELDS_DEFAULT_VALUES[inputs[i].name] == inputs[i].value;
				break;
		}
	}
	
	inputs = document.getElementsByTagName("select");
	for (let i=0; i<inputs.length; i++)
		inputs[i].value = inputs[i].options[0].value;
	
	describe_player_range('plrmin','plrmax','playersrangedescription');
}

function describe_player_range(input1_name, input2_name, output_name) {
	let min = document.getElementById(input1_name);
	let max = document.getElementById(input2_name);
	let output = document.getElementById(output_name);
	
	if (min && max && output) {
		let new_description = '';
		
		if (min.value != '')
			new_description += 'can be started with '+(min.value>1 ? (min.value+' or less players') : '1 player');
		
		if (max.value != '')		
			new_description += 
				(new_description!=''?'; ':'') + 
				'playable with ' + max.value + ' or more player' + (max.value!=1?'s':'');
		
		if (new_description != '')
			new_description = '(' + new_description + ')';

		output.innerHTML = new_description;	
	}
}
</script>
</body>
</html>
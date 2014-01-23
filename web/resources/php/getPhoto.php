<?php
  
  /********************************************************************/
  /* USAGE :																													*/
  /* getPhoto.php?																										*/
  /*		photoURL= 						[Nom de la photo, ie: toto.jpg] 			*/
  /*  	&photoResampledW=				[Largeur MAX du Jpeg de sortie				*/
  /*		&photoResampledH=				[Hauteur MAX du Jpeg de sortie]				*/
  /*		&photoResampledQuality=	[Qualité du Jpeg de sortie de 0 à 100]*/
  /*																																	*/
  /*  Retourne un Jpeg Fittant dans photoResampledW & photoResampledH */
  /*																																	*/
  /********************************************************************/
    
	$f_error = "NO_ERROR";

	//Récupération des paramètres
	$photoRep = "../../images/res1/";	/* Repertoire des photos relativement au script */
																		/* Contenant les photos de plus grande résolution: on est sûr qu'elles y sont */	

	
	$localChecksum =  md5( strval( $photoResampledW*($photoResampledH+17)-$photoResampledQuality ) );
	//Checksum non valide
	if( $localChecksum != $crc || strpos(" " . $photoURL,"/") != false){ //Rajout d'un espac avant sinon bug quand "/" est en première position
		$f_error = "Erreur de redondance cyclique !";
	}
	else{
		
		// lecture de l'image
		$photoURL = trim( $photoURL ); //On retire les caractères vides et de retour à la ligne
		$photo = @imagecreatefromjpeg( $photoRep . $photoURL  );
	
		//Le fichier a ete trouve 
		if( $photo != false )
		{
			$photoX = imagesx($photo);
			$photoY = imagesy($photo);
			$photoRatio = $photoX/$photoY;
		
			$newHeight = $photoY;
		
			if( $photoX > $photoResampledW )
			{
				$newHeight = $photoResampledW / $photoRatio;
				$newWidth = $photoResampledW;
				$fResized = 1;
			}
			if( $newHeight > $photoResampledH )
			{
				$newHeight = $photoResampledH;
				$newWidth = $newHeight * $photoRatio;
				$fResized = 1;
			}
			if( $fResized == 0)
			{
				$newHeight = $photoY;
				$newWidth = $photoX;
			}
				
			$photoResampledW = $newWidth;
			$photoResampledH = $newHeight;
		
			if( $fResized == 1)
			{		
				$photoResampled = @imagecreatetruecolor( $photoResampledW , $photoResampledH  );
				@imagecopyresampled($photoResampled , $photo,
									0, 0, 0, 0,
									$photoResampledW, $photoResampledH, imagesx( $photo ), imagesy( $photo )  );
			}
			else{ //Pas la peine de resampler si > tailleMax
				$photoResampled = $photo;
			}
		}
		//Photo inexistante
		else{
			$f_error = "Not found: " . $photoRep . $photoURL;
		}
	}
		
	//Si erreur creer un code explicitant l'erreur en gif
	if($f_error != "NO_ERROR")
	{		
		$photoResampled = @imagecreate(1024, 120);
		$bgColor 	= @imagecolorallocate($photoResampled, 0, 0, 0);
		$textcolor 	= @imagecolorallocate($photoResampled, 200, 20, 0);
		imagestring($photoResampled, 7, 10, 40, $f_error, $textcolor);	
	}
	
	
	
	//Envoi de l'image au script appelant
	// pas de mise en cache de l'image
	header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");
	header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");
	header("Cache-Control: no-cache, must-revalidate");
	header("Pragma: no-cache");
	Header("Content-type: image/jpeg");
	imagejpeg( $photoResampled , NULL, $photoResampledQuality );
	
?>

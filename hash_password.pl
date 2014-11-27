package MyApp::Util;

use strict;
use warnings;

# again, based on DBIx::Class::EncodedColumn::Crypt::Eksblowfish::Bcrypt

my $cost = 8;
my $nul  = 0;

$nul = $nul ? 'a' : '';
$cost = sprintf("%02i", 0+$cost);

my $settings_base = join('','$2',$nul,'$',$cost, '$');

sub check_password { hash_password($_[0], $_[1]) eq $_[1] }

sub hash_password {
   my ($plain_text, $settings_str) = @_;
   require Crypt::Eksblowfish::Bcrypt;

   unless ( $settings_str ) {
      my $salt = join('', map { chr(int(rand(256))) } 1 .. 16);
      $salt = Crypt::Eksblowfish::Bcrypt::en_base64( $salt );
      $settings_str =  $settings_base.$salt;
   }
   return Crypt::Eksblowfish::Bcrypt::bcrypt($plain_text, $settings_str);
}

# Maintainer: AnzoDK <anton@rosenoern-productions.dk>
pkgname="stoopiddb-header"
provides=('stoopiddb')
pkgver=0.2.1.1
pkgrel=1
pkgdesc="Simple and dirty database header for cpp projects"
arch=('any')
url="https://github.com/AnzoDK/StoopidDB"
makedepends=('git')
source=("git+https://github.com/AnzoDK/StoopidDB.git#tag=v${pkgver}")
license=('BSD2')
md5sums=('SKIP')
package()
{
   cd StoopidDB
   mkdir -p "$pkgdir/usr/include"
   install -D ./stoopidDB.h "$pkgdir/usr/include/stoopidDB.h"
   install -D ./LICENSE "${pkgdir}"/usr/share/licenses/$pkgname/LICENSE   
}


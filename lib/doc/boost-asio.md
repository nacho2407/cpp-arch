# Boost.Asio

&nbsp;크로스 플랫폼을 지향하는 C++ 표준은, 지속적으로 발전하여 지금은 멀티 쓰레딩이나 Boost C++ Libraries에 포함된 여러 편리한 기능들을 포함하고 있지만 여전히 네트워크 관련 기능은 플랫폼에 종속적이고 명시된 표준이 없다.

&nbsp;일반적으로 많이 사용하는 Boost.Asio 라이브러리는 많이 사용되는 플랫폼들(Microsoft Windows, POSIX)을 지원하기에 플랫폼 독립적인 C++ 코드를 작성할 수 있어 C++ 비동기 네트워크 프로그래밍에서 많이 사용하는 라이브러리다.


## Boost C++ Libraries

&nbsp;[Boost C++ Libraries](https://www.boost.org/)(이하 Boost)는 C++ 생태계에서 매우 많이 사용되는 플랫폼 독립적 라이브러리 집합으로, Boost.Asio 뿐 아니라 C++ 프로그래밍에 도움이 될 수 있는 많은 라이브러리들을 포함하고 있다. Boost에 포함된 기능이 C++ 표준에 포함되는 것들이 많은데, 대표적으로 [스마트 포인터](../../std/doc/smart_pointers.md), [정규식](../../std/doc/regular_expressions.md), [함수 객체](../../std/doc/functional.md) 등이 있다. 이는 Boost 개발 커뮤니티의 많은 개발자가 C++ 표준 위원회에 속해 있기 때문이기도 한데, Boost는 현재 사실상 C++ 표준의 기능 시험장으로 여겨지는 경우도 있다.


&nbsp;Boost는 하나의 라이브러리가 아닌 라이브러리 집합이기 때문에 그 크기도 매우 방대한데, 때문에 실제로 Boost를 다운받을 때는 빌드할 수 있는 코드만 포함되어 있고, 일부 헤더 전용 라이브러리(대표적으로 Boost.Asio)를 제외하면 대부분의 라이브러리를 `b2`(Boost.Build)를 이용해 직접 빌드하여 사용해야 한다.


&nbsp;Microsoft Windows 기준으로 Boost를 설치하는 방법은 다음과 같다. Boost 설치 위치를 환경 변수 Path에 등록해둔 후, 우선 `bootstrap.bat`을 이용하여 `b2`를 빌드한다. Microsoft Windows 기준으로 기본 툴체인이 MSVC로 되어있기 때문에 LLVM(`clang`)이나 mingw-w64(`gcc`)를 사용하는 경우 툴체인 정보를 추가로 넘겨주어야 한다. 이후 `b2`를 이용해 필요한 라이브러리를 빌드하는데, Boost.Asio 사용에 필요한 필수 라이브러리인 Boost.System는 사실상 대부분의 라이브러리에서 사용하므로 빌드해두는 것이 좋다.

```Shell
b2 -j9 toolset=clang  --with-system stage
```

&nbsp;사용할 라이브러리는 `--with-<라이브러리 이름>`으로 명시한다. 멀티 쓰레드를 이용해 빌드하려면 `-j<생성할 쓰레드 수>` 옵션과 함께 사용한다. 마지막의 `stage`는 라이브러리를 빌드할 폴더 이름으로, 원한다면 다른 이름으로 사용해도 된다.


&nbsp;이후 Boost와 함께 빌드하기 위해 컴파일 시 `-I<Boost 설치 경로>`, `-L<빌드한 Boost 라이브러리 경로(일반적으로 stage)>`를 추가한다.


*작성중 - ## Context: context의 의미, boost::asio::io_context ## TCP Networking: boost::asio::ip::tcp, boost::asio::ip::tcp::socket, boost::asio::ip::tcp::endpoint, boost::asio::ip::tcp::acceptor, boost::asio::ip::tcp::acceptor::async_accept, boost::asio::async_read_some, boost::asio::async_write, boost::asio::post, boost::asio::strand, boost::asio::buffer, 데이터 수신을 위한 boost::asio::buffer, boost::asio::ip::tcp::resolver, boost::asio::ip::tcp::resolver::resolve, boost::asio::connect, ## Threading: boost::asio::thread_pool ## Error: boost::system::error_code, boost::system::error_code의 종류 ## UDP Networking*

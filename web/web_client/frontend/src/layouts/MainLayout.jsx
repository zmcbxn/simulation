import { Outlet } from 'react-router-dom'
import Navbar from '../components/common/Navbar';

const MainLayout = () => {
  return (
    <div className="app-container">
      <Navbar />
      <main className="content">
          {/* Outlet 은 위에서 설정한 자식 라우트들이 갈아끼워지는 장소입니다 */}
          <Outlet />
      </main>
      <footer></footer>
    </div>
  )
}

export default MainLayout;
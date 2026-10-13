import {StrictMode, useState} from 'react';
import {createRoot} from 'react-dom/client';

import '@gravity-ui/uikit/styles/fonts.css';
import '@gravity-ui/uikit/styles/styles.css';

import {ThemeProvider, type Theme} from '@gravity-ui/uikit';

import App from './App.tsx';
import './index.css';

function Root() {
  const [theme, setTheme] = useState<Theme>('system');

  return (
    <ThemeProvider theme={theme}>
      <App theme={theme} onThemeChange={setTheme} />
    </ThemeProvider>
  );
}

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <Root />
  </StrictMode>,
);

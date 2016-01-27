% LaTeX class for capstone documentation

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{capstonedoc}[2016/01/23 Capstone Documentation Class]

% Inherit from article class
\LoadClass[twocolumn]{article}
% Handle class options
\DeclareOption{onecolumn}{\OptionNotUsed}
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{article}}
\ProcessOptions\relax

\usepackage{ragged2e}
\usepackage{color}
\usepackage{courier}
\usepackage{listings}

% Definitions for code listings
\definecolor{mygreen}{rgb}{0,0.6,0}
\definecolor{mygray}{rgb}{0.5,0.5,0.5}
\definecolor{mymauve}{rgb}{0.58,0,0.82}

\lstset{ %
  backgroundcolor=\color{white},   % choose the background color
  basicstyle=\footnotesize\ttfamily,% size and family of fonts used for the code
  breaklines=true,                 % automatic line breaking only at whitespace
  captionpos=t,                    % sets the caption-position to bottom
  commentstyle=\color{mygreen},    % comment style
  escapeinside={\%*}{*)},          % if you want to add LaTeX within your code
  keywordstyle=\color{blue},       % keyword style
  stringstyle=\color{mymauve},     % string literal style
}

% Macro to generate title page header
% (Override macro in article class)
\renewcommand{\maketitle}{
  \twocolumn[%
    {
      \fontsize{1.2cm}{1.5cm}%
      \fontfamily{phv}%
      \fontseries{b}%
      \fontshape{sl}%
      \selectfont\normalcolor%
      \begin{flushleft}%
        \@title%
      \end{flushleft}%
    }
    {
      \noindent%
      \@author%
      \hfill%
      \@date%
    }
    \noindent\rule{\textwidth}{0.5pt}
    \vspace{5mm}
  ]
}
